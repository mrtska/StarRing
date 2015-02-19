/*
[Module tag_parse.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/04
*/

#include <internal/tag_parse.h>
#include <internal/stdio.h>
#include <multiboot2.h>

#include <mem/alloc.h>


static struct multiboot_tag_mmap* mb_tag_mmap;


//
unsigned long get_max_physical_address(void) {


	unsigned long memory_size = 0;
	struct multiboot_tag_mmap *tag = mb_tag_mmap;

	multiboot_memory_map_t *mmap;
	for(mmap = ((struct multiboot_tag_mmap *) tag)->entries; (multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size; mmap = (multiboot_memory_map_t*) ((unsigned long) mmap + ((struct multiboot_tag_mmap*) tag)->entry_size)) {

		int type = mmap->type;
		unsigned long addr = mmap->addr;
		unsigned long len = mmap->len;
		unsigned long size = addr + len;

		kprintf("type %d, addr %p, len %p\n", type, addr, len);
		memory_size = (size > memory_size) ? size : memory_size;
	}
	return memory_size;
}

void parse_mmap(void) {

	struct multiboot_tag_mmap *tag = mb_tag_mmap;

	multiboot_memory_map_t *mmap;
	for(mmap = ((struct multiboot_tag_mmap *) tag)->entries; (multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size; mmap = (multiboot_memory_map_t*) ((unsigned long) mmap + ((struct multiboot_tag_mmap*) tag)->entry_size)) {

		int type = mmap->type;
		unsigned long addr = mmap->addr;
		unsigned long len = mmap->len;

		switch(type) {
		case MULTIBOOT_MEMORY_RESERVED:
		case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
		case MULTIBOOT_MEMORY_NVS:

			alloc_blocks(addr, len);
			break;
		case MULTIBOOT_MEMORY_BADRAM:

			kprintf("Bad memory found.\n");
			break;
		}
	}
}

void tag_parse(unsigned long addr) {

	if(addr & 7) {
		kprintf("Unaligned mbi: 0x%X\n", addr);
		return;
	}

	unsigned int size = *(unsigned int*) addr;
	kprintf("addr size is: %d\n", size);

	struct multiboot_tag *tag;
	for(tag = (struct multiboot_tag*) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7))) {
		//kprintf("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
		switch(tag->type) {
		case MULTIBOOT_TAG_TYPE_CMDLINE:
			kprintf("Command line = %s\n", ((struct multiboot_tag_string*) tag)->string);
			break;
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
			kprintf("Boot loader name = %s\n", ((struct multiboot_tag_string*) tag)->string);
			break;
		case MULTIBOOT_TAG_TYPE_MODULE:
			kprintf("Module at 0x%x-0x%x. Command line %s\n", ((struct multiboot_tag_module*) tag)->mod_start, ((struct multiboot_tag_module*) tag)->mod_end, ((struct multiboot_tag_module*) tag)->cmdline);
			break;
		case MULTIBOOT_TAG_TYPE_BOOTDEV:
			kprintf("Boot device 0x%X, partition %X, sub-partition %X\n", ((struct multiboot_tag_bootdev*) tag)->biosdev, ((struct multiboot_tag_bootdev*) tag)->slice, ((struct multiboot_tag_bootdev*) tag)->part);
			break;
		case MULTIBOOT_TAG_TYPE_MMAP: {

			mb_tag_mmap = (struct multiboot_tag_mmap*) tag;
			break;
		}
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
			/*multiboot_uint32_t color;
			 unsigned i;
			 struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
			 void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;
			 switch(tagfb->common.framebuffer_type) {
			 case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
			 unsigned best_distance, distance;
			 struct multiboot_color *palette;
			 palette = tagfb->framebuffer_palette;
			 color = 0;
			 best_distance = 4 * 256 * 256;
			 for(i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
			 distance = (0xff - palette[i].blue) * (0xff - palette[i].blue) + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
			 if(distance < best_distance) {
			 color = i;
			 best_distance = distance;
			 }
			 }
			 }
			 break;
			 case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			 color = ((1 << tagfb->framebuffer_blue_mask_size) - 1) << tagfb->framebuffer_blue_field_position;
			 break;
			 case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			 color = '\\' | 0x0100;
			 break;
			 default:
			 color = 0xffffffff;
			 break;
			 }
			 for(i = 0; i < tagfb->common.framebuffer_width && i < tagfb->common.framebuffer_height; i++) {
			 switch(tagfb->common.framebuffer_bpp) {
			 case 8: {
			 multiboot_uint8_t *pixel = fb + tagfb->common.framebuffer_pitch * i + i;
			 *pixel = color;
			 }
			 break;
			 case 15:
			 case 16: {
			 multiboot_uint16_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 2 * i;
			 *pixel = color;
			 }
			 break;
			 case 24: {
			 multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 3 * i;
			 *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
			 }
			 break;
			 case 32: {
			 multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
			 *pixel = color;
			 }
			 break;
			 }
			 }
			 break;
			 }*/
		}
		}
	}

}

