/*
 * font.c
 *
 *  Created on: 2014/10/16
 *      Author: StarRing
 */

#include <internal/stdio.h>
#include <font.h>
#include <drivers/fat32.h>
#include <slab.h>
#include <stdio.h>

void font_file_init(char *path, struct font_info *info) {

	//kprintf("path = %s\n", path);
	FILE *font = fopen(path, "r");

	//kprintf("name = %s, length = %d, pointer = %X\n", "IPAEXG.TTF", font->count, font->ptr);

	fread(0, 0, 0, font);

	struct table_directory *directory = malloc(sizeof(struct table_directory));
	directory->sfnt_version = FONT_READ32(font);
	directory->numTables = FONT_READ16(font);
	directory->searchRange = FONT_READ16(font);
	directory->entrySelector = FONT_READ16(font);
	directory->rangeShift = FONT_READ16(font);
	info->table = directory;

	//kprintf("sfnt_version = %X\n", font->base_ptr);

	int i;
	for(i = 0; i < directory->numTables; i++) {

		struct table_tag tags;
		tags.tag = FONT_READ32(font);
		tags.checksum = FONT_READ32(font);
		tags.offset = FONT_READ32(font);
		tags.length = FONT_READ32(font);
		char *tag = (char*) &tags.tag;

		//ビッグエンディアンに直す
		char t1 = tag[3];
		char t2 = tag[2];
		tag[3] = tag[0];
		tag[2] = tag[1];
		tag[1] = t2;
		tag[0] = t1;

		//kprintf("TAG = %c%c%c%c, length = %X, offset = %X\n", tag[0] & 0xFF, tag[1] & 0xFF, tag[2] & 0xFF, tag[3] & 0xFF, tags.length, tags.offset);

		//font_info構造体にそれぞれのタグのポインタを格納していく
		if(strncmp(tag, FONT_TAG_BASE, 4) == 0) {

			struct BASE_header *base = (struct BASE_header*) (tags.offset + font->base_ptr);
			info->base = base;
		} else if(strncmp(tag, FONT_TAG_GDEF, 4) == 0) {

			struct GDEF_header *gdef = (struct GDEF_header*) (tags.offset + font->base_ptr);
			info->gdef = gdef;
		} else if(strncmp(tag, FONT_TAG_GPOS, 4) == 0) {

			struct GPOS_header *gpos = (struct GPOS_header*) (tags.offset + font->base_ptr);
			info->gpos = gpos;
		} else if(strncmp(tag, FONT_TAG_OS2, 4) == 0) {

			struct OS2_data *os2 = (struct OS2_data*) (tags.offset + font->base_ptr);
			info->os2 = os2;
		} else if(strncmp(tag, FONT_TAG_CMAP, 4) == 0) {

			struct CMAP_header *cmap = (struct CMAP_header*) (tags.offset + font->base_ptr);
			info->cmap = cmap;
		} else if(strncmp(tag, FONT_TAG_CVT, 4) == 0) {

		} else if(strncmp(tag, FONT_TAG_FPGM, 4) == 0) {

		} else if(strncmp(tag, FONT_TAG_GASP, 4) == 0) {

			struct GASP_header *gasp = (struct GASP_header*) (tags.offset + font->base_ptr);
			info->gasp = gasp;
		} else if(strncmp(tag, FONT_TAG_GLYF, 4) == 0) {

			struct GLYF_header *glyf = (struct GLYF_header*) (tags.offset + font->base_ptr);
			info->glyf = glyf;
		} else if(strncmp(tag, FONT_TAG_HEAD, 4) == 0) {

			struct HEAD_header *head = (struct HEAD_header*) (tags.offset + font->base_ptr);
			info->head = head;
		} else if(strncmp(tag, FONT_TAG_HHEA, 4) == 0) {

			struct HHEA_header *hhea = (struct HHEA_header*) (tags.offset + font->base_ptr);
			info->hhea = hhea;
		} else if(strncmp(tag, FONT_TAG_HMTX, 4) == 0) {

		} else if(strncmp(tag, FONT_TAG_LOCA, 4) == 0) {

		} else if(strncmp(tag, FONT_TAG_MAXP, 4) == 0) {

			struct MAXP_header *maxp = (struct MAXP_header*) (tags.offset + font->base_ptr);
			info->maxp = maxp;
		} else if(strncmp(tag, FONT_TAG_NAME, 4) == 0) {

			struct NAME_header *name = (struct NAME_header*) (tags.offset + font->base_ptr);
			info->name = name;
		} else if(strncmp(tag, FONT_TAG_POST, 4) == 0) {

			struct POST_header *post = (struct POST_header*) (tags.offset + font->base_ptr);
			info->post = post;
		} else if(strncmp(tag, FONT_TAG_PREP, 4) == 0) {

		} else if(strncmp(tag, FONT_TAG_VHEA, 4) == 0) {

			struct VHEA_header *vhea = (struct VHEA_header*) (tags.offset + font->base_ptr);
			info->vhea = vhea;
		} else if(strncmp(tag, FONT_TAG_VMTX, 4) == 0) {

		}
	}

	fseek(font, (long) info->cmap, SEEK_SET);


	struct CMAP_header *cmap = malloc(sizeof(struct CMAP_header));
	cmap->version = FONT_READ16(font);
	cmap->numTables = FONT_READ16(font);
	//kprintf("cmap = %X, version = %X, numTables = %u\n", info->cmap, cmap->version, cmap->numTables);

	struct EncordTable *table = malloc(sizeof(struct EncordTable) * cmap->numTables);

	for(i = 0; i < cmap->numTables; i++) {

		table[i].PlatformID = FONT_READ16(font);
		table[i].EncordingID = FONT_READ16(font);
		table[i].offset = FONT_READ32(font);
	}
	for(i = 0; i < cmap->numTables; i++) {

		//kprintf("platformID = %X, encordingID = %X, offset = %X\n", table[i].PlatformID, table[i].EncordingID, table[i].offset);
		fseek(font, (long) info->cmap, SEEK_SET);

		fseek(font, table[i].offset, SEEK_CUR);
		unsigned short format = FONT_READ16(font);
		//kprintf("format = %u\n", format);
		if(format == 12) {

			struct Format12 format12;
			format12.format = format;
			format12.reserved = FONT_READ16(font);
			format12.length = FONT_READ32(font);
			format12.language = FONT_READ32(font);
			format12.nGroups = FONT_READ32(font);

			info->glyph_unicode = malloc(sizeof(unsigned int) * format12.nGroups);

			int j;
			for(j = 0; j < format12.nGroups; j++) {

				unsigned int start = FONT_READ32(font);
				unsigned int end = FONT_READ32(font);
				unsigned int start_glyph = FONT_READ32(font);
				int k;
				for(k = start; k <= end; k++) {

					info->glyph_unicode[start_glyph + k - start] = k;
				}
			}
			//kprintf("Format12 Found.\n");
			break;
		}
	}
}

void load_glyf(struct font_info *info, struct Simple_Glyph_Description *glyph) {

	;
	unsigned char *p = (unsigned char*) info->glyf;
	p += sizeof(struct GLYF_header);
	glyph->endPtsOfContours = (unsigned short*) p;
	p += sizeof(unsigned short) * FONT_READ16(info->glyf->numberOfCntours);
	glyph->instructionLength = *(unsigned short*) p;
	p += sizeof(unsigned short);
	glyph->instructions = p;
	p += FONT_READ16(glyph->instructionLength);
	glyph->flags = p;
	glyph->flagLength = FONT_READ16(glyph->endPtsOfContours[FONT_READ16(info->glyf->numberOfCntours) - 1]) + 1;
	p += glyph->flagLength;
	glyph->xCoordinates = (short*) p;
	int i;
	int x = 0;
	int y = 0;
	for(i = 0; i < glyph->flagLength; i++) {

		unsigned char flag = glyph->flags[i];
		if((flag & FONT_GLYF_SIMPLE_THIS_X_IS_SAME) && !(flag & FONT_GLYF_SIMPLE_X_SHORT_VECTOR)) {

		} else {

			x++;
		}
		if((flag & FONT_GLYF_SIMPLE_THIS_Y_IS_SAME) && !(flag & FONT_GLYF_SIMPLE_Y_SHORT_VECTOR)) {

		} else {

			y++;
		}

	}

	glyph->xCoordinates = malloc(x * 2);
	glyph->yCoordinates = malloc(y * 2);
	x = 0;
	y = 0;
	for(i = 0; i < glyph->flagLength; i++) {

		unsigned char flag = glyph->flags[i];
		if((flag & FONT_GLYF_SIMPLE_THIS_X_IS_SAME) && !(flag & FONT_GLYF_SIMPLE_X_SHORT_VECTOR)) {

		} else {

			if(flag & FONT_GLYF_SIMPLE_X_SHORT_VECTOR) {

				glyph->xCoordinates[x] = (*(char*) p) & 0xFF;
				p += sizeof(char);
			} else {
				glyph->xCoordinates[x] = FONT_READ16(*(short*) p) & 0xFFFF;
				p += sizeof(short);
			}

			x++;
		}
	}

	for(i = 0; i < glyph->flagLength; i++) {

		unsigned char flag = glyph->flags[i];
		if((flag & FONT_GLYF_SIMPLE_THIS_Y_IS_SAME) && !(flag & FONT_GLYF_SIMPLE_Y_SHORT_VECTOR)) {

		} else {

			if(flag & FONT_GLYF_SIMPLE_Y_SHORT_VECTOR) {

				glyph->yCoordinates[y] = (*(char*) p) & 0xFF;
				p += sizeof(char);
			} else {
				glyph->yCoordinates[y] = FONT_READ16(*(short*) p) & 0xFFFF;
				p += sizeof(short);
			}

			y++;
		}
	}

}


unsigned int get_glyphID(struct font_info *info, unsigned short c) {



	return 0;
}


















