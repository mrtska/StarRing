/*
[Module vmware.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/19
*/

#include <drivers/vmware.h>
#include <drivers/pci.h>
#include <apic.h>
#include <desc.h>
#include <ioapic.h>

extern unsigned long vram;
extern void svga_handler(void);

struct svga_device svga_device;

void *svga_fifo_reserve(unsigned int bytes) {

	volatile unsigned int *fifo = svga_device.fifoMem;
	unsigned int max = fifo[SVGA_FIFO_MAX];
	unsigned int min = fifo[SVGA_FIFO_MIN];
	unsigned int next_cmd = fifo[SVGA_FIFO_NEXT_CMD];
	bool reserveable = svga_has_fifo_capability(VMWARE_SVGA_FIFO_CAP_RESERVE);

	if(bytes > sizeof svga_device.fifo.bounceBuffer || bytes > (max - min)) {

		kprintf("[%s] FIFO Command too large\n", __FUNCTION__);
		//return NULL;
	}

	if(bytes % sizeof(unsigned int)) {

		kprintf("[%s]ERROR!2\n", __FUNCTION__);
		return NULL;
	}

	if(svga_device.fifo.reservedSize != 0) {

		kprintf("[%s]ERROR!3\n", __FUNCTION__);
		return NULL;
	}

	svga_device.fifo.reservedSize = bytes;

	while(true) {

		unsigned int stop = fifo[SVGA_FIFO_STOP];
		bool reserve_in_place = false;
		bool needBounce = false;

		if(next_cmd >= stop) {

			if(next_cmd + bytes < max || (next_cmd + bytes == max && stop > min)) {

				reserve_in_place = true;
			} else if((max - next_cmd) + (stop - min) <= bytes) {

			} else {

				needBounce = true;
			}
		} else {

			if(next_cmd + bytes < stop) {

				reserve_in_place = true;
			}
		}

		if(reserve_in_place) {

			if(reserveable || bytes <= sizeof(unsigned int)) {

				svga_device.fifo.usingBounceBuffer = false;
				if(reserveable) {

					fifo[SVGA_FIFO_RESERVED] = bytes;
				}
				return next_cmd + (unsigned char*) fifo;
			} else {

				needBounce = true;
			}
		}

		if(needBounce) {

			svga_device.fifo.usingBounceBuffer = true;
			return svga_device.fifo.bounceBuffer;
		}
	}
}

unsigned int svga_insert_fence(void) {

	unsigned int fence;

	struct {

		unsigned int id;
		unsigned int fence;
	}__attribute__((packed)) *cmd;

	if(svga_has_fifo_capability(VMWARE_SVGA_FIFO_CAP_FENCE)) {

		return 1;
	}

	if(svga_device.fifo.nextFence == 0) {

		svga_device.fifo.nextFence = 1;
	}
	fence = svga_device.fifo.nextFence++;

	cmd = svga_fifo_reserve(sizeof *cmd);
	cmd->id = SVGA_CMD_FENCE;
	cmd->fence = fence;
	svga_fifo_commit_all();

	return fence;
}

void svga_fifo_commit(unsigned int bytes) {

	volatile unsigned int *fifo = svga_device.fifoMem;
	unsigned int max = fifo[SVGA_FIFO_MAX];
	unsigned int min = fifo[SVGA_FIFO_MIN];
	unsigned int next_cmd = fifo[SVGA_FIFO_NEXT_CMD];
	bool reserveable = svga_has_fifo_capability(VMWARE_SVGA_FIFO_CAP_RESERVE);

	if(svga_device.fifo.reservedSize == 0) {

		kprintf("[%s] Must be call svga_fifo_reserve before\n", __FUNCTION__);
		return;
	}

	svga_device.fifo.reservedSize = 0;
	if(svga_device.fifo.usingBounceBuffer) {

		unsigned char *buffer = svga_device.fifo.bounceBuffer;

		if(reserveable) {

			unsigned int chunk_size = MIN(bytes, max - next_cmd);
			fifo[SVGA_FIFO_RESERVED] = bytes;
			memcpy(next_cmd + (unsigned char*) fifo, buffer, chunk_size);
			memcpy(min + (unsigned char*) fifo, buffer + chunk_size, bytes - chunk_size);
		} else {

			unsigned int *dword = (unsigned int*) buffer;

			while(bytes > 0) {

				fifo[next_cmd / sizeof *dword] = *dword++;
				next_cmd += sizeof *dword;
				if(next_cmd == max) {

					next_cmd = min;
				}

				fifo[SVGA_FIFO_NEXT_CMD] = next_cmd;
				bytes -= sizeof *dword;
			}
		}
	}

	if(!svga_device.fifo.usingBounceBuffer || reserveable) {

		next_cmd += bytes;
		if(next_cmd >= max) {

			next_cmd -= max - min;
		}
		fifo[SVGA_FIFO_NEXT_CMD] = next_cmd;
	}

	if(reserveable) {

		fifo[SVGA_FIFO_RESERVED] = 0;
	}
}

void svga_sync_to_fence(unsigned int fence) {

	if(!fence) {

		return;
	}

	if(!svga_has_fifo_capability(VMWARE_SVGA_FIFO_CAP_FENCE)) {

		svga_write_value(SVGA_REG_SYNC, 1);
		while(svga_read_value(SVGA_REG_BUSY) != false)
			;
		return;
	}

	if(svga_has_fence_passed(fence)) {

		return;
	}
	if(svga_is_fifo_reg_valid(SVGA_FIFO_FENCE_GOAL) && (svga_device.capabilities & VMWARE_SVGA_CAP_IRQMASK)) {

		svga_device.fifoMem[SVGA_FIFO_FENCE_GOAL] = fence;
		svga_write_value(SVGA_REG_IRQMASK, VMWARE_SVGA_IRQFLAG_FENCE_GOAL);

		svga_clear_irq();

		if(!svga_has_fence_passed(fence)) {

			svga_ring_door_bell();
		}
	} else {

		svga_write_value(SVGA_REG_IRQMASK, 0);

		bool busy = true;

		svga_write_value(SVGA_REG_SYNC, 1);
		while(!svga_has_fence_passed(fence) && busy) {

			busy = (svga_read_value(SVGA_REG_BUSY) != 0);
		}
	}
}

void svga_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {

	struct svga_fifo_cmd_update *cmd = svga_fifo_reservecmd(SVGA_CMD_UPDATE, sizeof *cmd);
	cmd->x = x;
	cmd->y = y;
	cmd->width = width;
	cmd->height = height;
	svga_fifo_commit_all();
}
/*
void vmware_fill_screen(struct graphics_context *ctx, unsigned int color, int width, int height) {

	unsigned int *fb = (unsigned int*) svga_device.fbMem;
	int x;
	int y;
	static unsigned int fence = 0;

	svga_sync_to_fence(fence);
	fence = svga_insert_fence();

	for(y = 0; y < ctx->height; y++) {

		unsigned int *row = fb;
		fb = (unsigned int*) (svga_device.pitch + (unsigned char*) fb);

		for(x = 0; x < ctx->width; x++) {

			*(row++) = color;
			svga_update(x, y, 1, 1);
		}
	}
}*/

void vmware_graphics_init(unsigned int x, unsigned int y) {

	struct pci_device *device = pci_find_device(VMWARE_SVGA_VENDOR_ID, VMWARE_SVGA_DEVICE_ID);
	svga_device.pciAddr = device;

	svga_device.ioBase = pci_conf_read(device, PCI_REGISTER_BAR0) & 0xFFFFFFFC;
	svga_device.fbMem = (unsigned char*) (unsigned long) (pci_conf_read(device, PCI_REGISTER_BAR1) & ~0xF);
	svga_device.fifoMem = (unsigned int*) (unsigned long) (pci_conf_read(device, PCI_REGISTER_BAR2) & ~0xF);

	kprintf("iobase = %X\n", svga_device.ioBase);
	svga_device.deviceVersionId = VMWARE_SVGA_VERSION_2;
	do {
		svga_write_value(SVGA_REG_ID, svga_device.deviceVersionId);
		if(svga_read_value(SVGA_REG_ID) == svga_device.deviceVersionId) {
			kprintf("!!!\n");
			break;
		} else {
			svga_device.deviceVersionId--;
		}
	} while(svga_device.deviceVersionId >= VMWARE_SVGA_ID_0);

	kprintf("SVGA ID = %X\n", svga_read_value(SVGA_REG_CAPABILITIES));

	svga_device.vramSize = svga_read_value(SVGA_REG_VRAM_SIZE);
	svga_device.fbSize = svga_read_value(SVGA_REG_FB_SIZE);
	svga_device.fifoSize = svga_read_value(SVGA_REG_MEM_SIZE);

	if(svga_device.deviceVersionId >= VMWARE_SVGA_ID_1) {
		svga_device.capabilities = svga_read_value(SVGA_REG_CAPABILITIES);
	}

	if(svga_device.capabilities & VMWARE_SVGA_CAP_IRQMASK) {

		unsigned int irq = pci_conf_read(device, PCI_REGISTER_INTERRUPT_PIN) & 0xFF;

		svga_write_value(SVGA_REG_IRQMASK, 0);

		outl(svga_device.ioBase + VMWARE_SVGA_IRQSTATUS_PORT, 0xFF);

		svga_clear_irq();

		set_intr_gate(irq + 0x20, svga_handler);

		trace();

	}

	svga_device.fifoMem[SVGA_FIFO_MIN] = SVGA_FIFO_NUM_REGS * sizeof(unsigned int);
	svga_device.fifoMem[SVGA_FIFO_MAX] = svga_device.fifoSize;
	svga_device.fifoMem[SVGA_FIFO_NEXT_CMD] = svga_device.fifoMem[SVGA_FIFO_MIN];
	svga_device.fifoMem[SVGA_FIFO_STOP] = svga_device.fifoMem[SVGA_FIFO_MIN];

	if(svga_has_fifo_capability(VMWARE_SVGA_CAP_EXTENDED_FIFO) && svga_is_fifo_reg_valid(SVGA_FIFO_GUEST_3D_HWVERSION)) {

		svga_device.fifoMem[SVGA_FIFO_GUEST_3D_HWVERSION] = SVGA3D_HWVERSION_CURRENT;
	}

	//svga_write_value(SVGA_REG_ENABLE, 1);
	svga_write_value(SVGA_REG_CONFIG_DONE, 1);

	svga_device.width = x;
	svga_device.height = y;
	svga_device.bpp = 32;

	svga_write_value(SVGA_REG_WIDTH, x);
	svga_write_value(SVGA_REG_HEIGHT, y);
	svga_write_value(SVGA_REG_BITS_PER_PIXEL, 32);
	svga_write_value(SVGA_REG_ENABLE, 1);
	svga_device.pitch = svga_read_value(SVGA_REG_BYTES_PER_LINE);

	io_apic_set_redirect(9, 0, 41);
}

void gmr_init(void) {

	//無理
}

void do_svga_handler(void) {

	while(1) asmv("hlt");

	trace();

	unsigned short port = svga_device.ioBase + VMWARE_SVGA_IRQSTATUS_PORT;
	unsigned int irqFlags = inl(port);
	outl(port, irqFlags);

	apic_eoi();
}

