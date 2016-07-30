/*
[Module vmware.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/19
*/

#pragma once

#include <system.h>
#include <drivers/pci.h>

#define VMWARE_SVGA_VENDOR_ID	0x15AD
#define VMWARE_SVGA_DEVICE_ID	0x0405

#define VMWARE_SVGA_BAR0_REGISTER	0x00001070
#define VMWARE_SVGA_BAR1_REGISTER	0xE0000000
#define VMWARE_SVGA_BAR2_REGISTER	0xE8000000
#define VMWARE_SVGA_BAR3_REGISTER	0x00000000

//BAR0ポート
#define VMWARE_SVGA_INDEX_PORT		0x0
#define VMWARE_SVGA_VALUE_PORT		0x1
#define VMWARE_SVGA_BIOS_PORT		0x2
#define VMWARE_SVGA_IRQSTATUS_PORT	0x8

#define VMWARE_SVGA_IRQFLAG_ANY_FENCE            0x1
#define VMWARE_SVGA_IRQFLAG_FIFO_PROGRESS        0x2
#define VMWARE_SVGA_IRQFLAG_FENCE_GOAL           0x4

#define VMWARE_SVGA_CAP_NONE               0x00000000
#define VMWARE_SVGA_CAP_RECT_COPY          0x00000002
#define VMWARE_SVGA_CAP_CURSOR             0x00000020
#define VMWARE_SVGA_CAP_CURSOR_BYPASS      0x00000040
#define VMWARE_SVGA_CAP_CURSOR_BYPASS_2    0x00000080
#define VMWARE_SVGA_CAP_8BIT_EMULATION     0x00000100
#define VMWARE_SVGA_CAP_ALPHA_CURSOR       0x00000200
#define VMWARE_SVGA_CAP_3D                 0x00004000
#define VMWARE_SVGA_CAP_EXTENDED_FIFO      0x00008000
#define VMWARE_SVGA_CAP_MULTIMON           0x00010000
#define VMWARE_SVGA_CAP_PITCHLOCK          0x00020000
#define VMWARE_SVGA_CAP_IRQMASK            0x00040000
#define VMWARE_SVGA_CAP_DISPLAY_TOPOLOGY   0x00080000
#define VMWARE_SVGA_CAP_GMR                0x00100000
#define VMWARE_SVGA_CAP_TRACES             0x00200000
#define VMWARE_SVGA_CAP_GMR2               0x00400000
#define VMWARE_SVGA_CAP_SCREEN_OBJECT_2    0x00800000

#define VMWARE_SVGA_GMR_NULL         ((unsigned int) -1)
#define VMWARE_SVGA_GMR_FRAMEBUFFER  ((unsigned int) -2)


#define VMWARE_SVGA_MAGIC         0x900000UL
#define VMWARE_SVGA_MAKE_ID(ver)  (VMWARE_SVGA_MAGIC << 8 | (ver))

#define VMWARE_SVGA_VERSION_2     2
#define VMWARE_SVGA_ID_2          VMWARE_SVGA_MAKE_ID(VMWARE_SVGA_VERSION_2)
#define VMWARE_SVGA_VERSION_1     1
#define VMWARE_SVGA_ID_1          VMWARE_SVGA_MAKE_ID(VMWARE_SVGA_VERSION_1)
#define VMWARE_SVGA_VERSION_0     0
#define VMWARE_SVGA_ID_0          VMWARE_SVGA_MAKE_ID(VMWARE_SVGA_VERSION_0)

#define VMWARE_SVGA_ID_INVALID    0xFFFFFFFF

#define VMWARE_SVGA_FIFO_CAP_NONE                  0
#define VMWARE_SVGA_FIFO_CAP_FENCE             (1<<0)
#define VMWARE_SVGA_FIFO_CAP_ACCELFRONT        (1<<1)
#define VMWARE_SVGA_FIFO_CAP_PITCHLOCK         (1<<2)
#define VMWARE_SVGA_FIFO_CAP_VIDEO             (1<<3)
#define VMWARE_SVGA_FIFO_CAP_CURSOR_BYPASS_3   (1<<4)
#define VMWARE_SVGA_FIFO_CAP_ESCAPE            (1<<5)
#define VMWARE_SVGA_FIFO_CAP_RESERVE           (1<<6)
#define VMWARE_SVGA_FIFO_CAP_SCREEN_OBJECT     (1<<7)
#define VMWARE_SVGA_FIFO_CAP_GMR2              (1<<8)
#define VMWARE_SVGA_FIFO_CAP_3D_HWVERSION_REVISED  VMWARE_SVGA_FIFO_CAP_GMR2
#define VMWARE_SVGA_FIFO_CAP_SCREEN_OBJECT_2   (1<<9)
#define VMWARE_SVGA_FIFO_CAP_DEAD              (1<<10)

#define SVGA3D_MAKE_HWVERSION(major, minor)      (((major) << 16) | ((minor) & 0xFF))
#define SVGA3D_MAJOR_HWVERSION(version)          ((version) >> 16)
#define SVGA3D_MINOR_HWVERSION(version)          ((version) & 0xFF)

enum {
   SVGA3D_HWVERSION_WS5_RC1   = SVGA3D_MAKE_HWVERSION(0, 1),
   SVGA3D_HWVERSION_WS5_RC2   = SVGA3D_MAKE_HWVERSION(0, 2),
   SVGA3D_HWVERSION_WS51_RC1  = SVGA3D_MAKE_HWVERSION(0, 3),
   SVGA3D_HWVERSION_WS6_B1    = SVGA3D_MAKE_HWVERSION(1, 1),
   SVGA3D_HWVERSION_FUSION_11 = SVGA3D_MAKE_HWVERSION(1, 4),
   SVGA3D_HWVERSION_WS65_B1   = SVGA3D_MAKE_HWVERSION(2, 0),
   SVGA3D_HWVERSION_WS8_B1    = SVGA3D_MAKE_HWVERSION(2, 1),
   SVGA3D_HWVERSION_CURRENT   = SVGA3D_HWVERSION_WS8_B1,
};




enum {
	SVGA_REG_ID = 0,
	SVGA_REG_ENABLE = 1,
	SVGA_REG_WIDTH = 2,
	SVGA_REG_HEIGHT = 3,
	SVGA_REG_MAX_WIDTH = 4,
	SVGA_REG_MAX_HEIGHT = 5,
	SVGA_REG_DEPTH = 6,
	SVGA_REG_BITS_PER_PIXEL = 7,       // ゲストの現在のBPP
	SVGA_REG_PSEUDOCOLOR = 8,
	SVGA_REG_RED_MASK = 9,
	SVGA_REG_GREEN_MASK = 10,
	SVGA_REG_BLUE_MASK = 11,
	SVGA_REG_BYTES_PER_LINE = 12,
	SVGA_REG_FB_START = 13,            // 非推奨
	SVGA_REG_FB_OFFSET = 14,
	SVGA_REG_VRAM_SIZE = 15,
	SVGA_REG_FB_SIZE = 16,

	// IDが0だった場合、↑のレジスタ以外は実装されてない

	SVGA_REG_CAPABILITIES = 17,
	SVGA_REG_MEM_START = 18, // 非推奨
	SVGA_REG_MEM_SIZE = 19,
	SVGA_REG_CONFIG_DONE = 20, // Set when memory area configured
	SVGA_REG_SYNC = 21, // See "FIFO Synchronization Registers"
	SVGA_REG_BUSY = 22, // See "FIFO Synchronization Registers"
	SVGA_REG_GUEST_ID = 23, // Set guest OS identifier
	SVGA_REG_CURSOR_ID = 24, // 非推奨
	SVGA_REG_CURSOR_X = 25, // 非推奨
	SVGA_REG_CURSOR_Y = 26, // 非推奨
	SVGA_REG_CURSOR_ON = 27, // 非推奨
	SVGA_REG_HOST_BITS_PER_PIXEL = 28, // 非推奨
	SVGA_REG_SCRATCH_SIZE = 29, // Number of scratch registers
	SVGA_REG_MEM_REGS = 30, // Number of FIFO registers
	SVGA_REG_NUM_DISPLAYS = 31, // 非推奨
	SVGA_REG_PITCHLOCK = 32, // Fixed pitch for all modes
	SVGA_REG_IRQMASK = 33, // Interrupt mask

	// Legacy multi-monitor support
	SVGA_REG_NUM_GUEST_DISPLAYS = 34,// Number of guest displays in X/Y direction
	SVGA_REG_DISPLAY_ID = 35, // Display ID for the following display attributes
	SVGA_REG_DISPLAY_IS_PRIMARY = 36,// Whether this is a primary display
	SVGA_REG_DISPLAY_POSITION_X = 37,// The display position x
	SVGA_REG_DISPLAY_POSITION_Y = 38,// The display position y
	SVGA_REG_DISPLAY_WIDTH = 39, // The display's width
	SVGA_REG_DISPLAY_HEIGHT = 40, // The display's height

	// See "Guest memory regions" below.
	SVGA_REG_GMR_ID = 41,
	SVGA_REG_GMR_DESCRIPTOR = 42,
	SVGA_REG_GMR_MAX_IDS = 43,
	SVGA_REG_GMR_MAX_DESCRIPTOR_LENGTH = 44,

	SVGA_REG_TRACES = 45, // Enable trace-based updates even when FIFO is on
	SVGA_REG_GMRS_MAX_PAGES = 46, // Maximum number of 4KB pages for all GMRs
	SVGA_REG_MEMORY_SIZE = 47, // Total dedicated device memory excluding FIFO
	SVGA_REG_TOP = 48, // Must be 1 more than the last register

	SVGA_PALETTE_BASE = 1024, // Base of SVGA color map
};

enum {

	SVGA_FIFO_MIN = 0,
	SVGA_FIFO_MAX, // The distance from MIN to MAX must be at least 10K
	SVGA_FIFO_NEXT_CMD,
	SVGA_FIFO_STOP,
	SVGA_FIFO_CAPABILITIES = 4,
	SVGA_FIFO_FLAGS,
	SVGA_FIFO_FENCE,
	SVGA_FIFO_3D_HWVERSION,
	SVGA_FIFO_PITCHLOCK,
	SVGA_FIFO_CURSOR_ON, // Cursor bypass 3 show/hide register
	SVGA_FIFO_CURSOR_X, // Cursor bypass 3 x register
	SVGA_FIFO_CURSOR_Y, // Cursor bypass 3 y register
	SVGA_FIFO_CURSOR_COUNT, // Incremented when any of the other 3 change
	SVGA_FIFO_CURSOR_LAST_UPDATED,// Last time the host updated the cursor

	// Valid with SVGA_FIFO_CAP_RESERVE:
	SVGA_FIFO_RESERVED,
	SVGA_FIFO_CURSOR_SCREEN_ID,
	SVGA_FIFO_DEAD,

	SVGA_FIFO_3D_HWVERSION_REVISED,

	SVGA_FIFO_3D_CAPS = 32,
	SVGA_FIFO_3D_CAPS_LAST = 32 + 255,

	SVGA_FIFO_GUEST_3D_HWVERSION, // Guest driver's 3D version
	SVGA_FIFO_FENCE_GOAL, // Matching target for SVGA_IRQFLAG_FENCE_GOAL
	SVGA_FIFO_BUSY, // See "FIFO Synchronization Registers"

	SVGA_FIFO_NUM_REGS
};


enum {
   SVGA_CMD_INVALID_CMD           = 0,
   SVGA_CMD_UPDATE                = 1,
   SVGA_CMD_RECT_COPY             = 3,
   SVGA_CMD_DEFINE_CURSOR         = 19,
   SVGA_CMD_DEFINE_ALPHA_CURSOR   = 22,
   SVGA_CMD_UPDATE_VERBOSE        = 25,
   SVGA_CMD_FRONT_ROP_FILL        = 29,
   SVGA_CMD_FENCE                 = 30,
   SVGA_CMD_ESCAPE                = 33,
   SVGA_CMD_DEFINE_SCREEN         = 34,
   SVGA_CMD_DESTROY_SCREEN        = 35,
   SVGA_CMD_DEFINE_GMRFB          = 36,
   SVGA_CMD_BLIT_GMRFB_TO_SCREEN  = 37,
   SVGA_CMD_BLIT_SCREEN_TO_GMRFB  = 38,
   SVGA_CMD_ANNOTATION_FILL       = 39,
   SVGA_CMD_ANNOTATION_COPY       = 40,
   SVGA_CMD_DEFINE_GMR2           = 41,
   SVGA_CMD_REMAP_GMR2            = 42,
   SVGA_CMD_MAX
};

struct svga_device {
	struct pci_device *pciAddr;
	unsigned int ioBase;
	unsigned int *fifoMem;
	unsigned char *fbMem;
	unsigned int fifoSize;
	unsigned int fbSize;
	unsigned int vramSize;

	unsigned int deviceVersionId;
	unsigned int capabilities;

	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned int pitch;

	struct {
		unsigned int reservedSize;
		bool usingBounceBuffer;
		unsigned char bounceBuffer[1024 * 1024];
		unsigned int nextFence;
	} fifo;

	volatile struct {
		unsigned int pending;
		unsigned int switchContext;
		/* IntrContext   oldContext;
		 IntrContext   newContext;*/
		unsigned int count;
	} irq;

};

struct svga_fifo_cmd_update {

	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
};


extern struct svga_device svga_device;

void svga_fifo_commit(unsigned int bytes);

static __inline__ unsigned int svga_read_value(int reg) {

	outl(svga_device.ioBase + VMWARE_SVGA_INDEX_PORT, reg);
	return inl(svga_device.ioBase + VMWARE_SVGA_VALUE_PORT);
}
static __inline__ void svga_write_value(int reg, int value) {

	outl(svga_device.ioBase + VMWARE_SVGA_INDEX_PORT, reg);
	outl(svga_device.ioBase + VMWARE_SVGA_VALUE_PORT, value);
}



static __inline__ unsigned int svga_clear_irq(void) {

	unsigned int flags = 0;
	asmv("xchgl %0, %1" : "+r"(flags), "+m"(svga_device.irq.pending) :);
	return flags;
}

static __inline__ bool svga_has_fifo_capability(int cap) {

	return (svga_device.fifoMem[SVGA_FIFO_CAPABILITIES] & cap) != 0;
}

static __inline__  bool svga_is_fifo_reg_valid(int reg) {

	return svga_device.fifoMem[SVGA_FIFO_MIN] > (reg << 2);
}
static __inline__ void svga_ring_door_bell(void) {

	if(svga_is_fifo_reg_valid(SVGA_FIFO_BUSY) && svga_device.fifoMem[SVGA_FIFO_BUSY] == false) {


		svga_device.fifoMem[SVGA_FIFO_BUSY] = true;

		svga_write_value(SVGA_REG_SYNC, 1);
	}
}

static __inline__ void svga_fifo_full(void) {

	svga_write_value(SVGA_REG_SYNC, 1);
	svga_read_value(SVGA_REG_BUSY);
}


static __inline__ void svga_fifo_commit_all(void) {

	svga_fifo_commit(svga_device.fifo.reservedSize);
}

static __inline__ bool svga_has_fence_passed(unsigned int fence) {

	if(!fence) {

		return false;
	}

	if(!svga_has_fifo_capability(VMWARE_SVGA_FIFO_CAP_FENCE)) {

		return false;
	}

	return ((int) (svga_device.fifoMem[SVGA_FIFO_FENCE] - fence)) >= 0;
}
void *svga_fifo_reserve(unsigned int bytes);
static __inline__ void *svga_fifo_reservecmd(unsigned int type, unsigned int bytes) {

	unsigned int *cmd = svga_fifo_reserve(bytes + sizeof type);
	cmd[0] = type;
	return cmd + 1;
}





void vmware_graphics_init(unsigned int x, unsigned int y);
void svga_sync_to_fence(unsigned int fence);
unsigned int svga_insert_fence(void);
void svga_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
