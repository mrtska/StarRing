/*
 * font.h
 *
 *  Created on: 2014/10/16
 *      Author: StarRing
 */

#pragma once

#include <byteorder.h>


#define FONT_TAG_BASE	"BASE"
#define FONT_TAG_GDEF	"GDEF"
#define FONT_TAG_GPOS	"GPOS"
#define FONT_TAG_GSUB	"GSUB"
#define FONT_TAG_OS2	"OS/2"
#define FONT_TAG_CMAP	"cmap"
#define FONT_TAG_CVT	"cvt "
#define FONT_TAG_FPGM	"fpgm"
#define FONT_TAG_GASP	"gasp"
#define FONT_TAG_GLYF	"glyf"
#define FONT_TAG_HEAD	"head"
#define FONT_TAG_HHEA	"hhea"
#define FONT_TAG_HMTX	"hmtx"
#define FONT_TAG_LOCA	"loca"
#define FONT_TAG_MAXP	"maxp"
#define FONT_TAG_NAME	"name"
#define FONT_TAG_POST	"post"
#define FONT_TAG_PREP	"prep"
#define FONT_TAG_VHEA	"vhea"
#define FONT_TAG_VMTX	"vmtx"



//True Type Font データ型定義
typedef unsigned char BYTE;
typedef char CHAR;
typedef unsigned short USHORT;
typedef short SHORT;
typedef unsigned int ULONG;
typedef int LONG;
typedef int FIXED;
typedef char FUNIT;
typedef short FWORD;
typedef unsigned short UFWORD;
typedef short F2DOT14;

//フォントデータ関連の構造体は以下のマクロを通して参照すること ビッグエンディアン to リトルエンディアン
#define FONT_READ16(x)	conv_endian16(x)
#define FONT_READ32(x)	conv_endian32(x)



//フォントファイル テーブルディレクトリ
struct table_directory {

	FIXED sfnt_version;		//TTFバージョン
	USHORT numTables;		//テーブルの数
	USHORT searchRange;		//
	USHORT entrySelector;	//
	USHORT rangeShift;		//
};

//フォントファイル タグ
struct table_tag {

	ULONG tag;				//4字識別子
	ULONG checksum;			//チェックサム
	ULONG offset;			//オフセット
	ULONG length;			//サイズ
};




//---BASE---
//ベーススクリプトレコード
struct base_script_record {

	unsigned char BaseScriptTag[4];
	unsigned short BaseScript;
};

//ベーススクリプトリスト
struct base_script_list {

	unsigned short BaseScriptCount;
	struct base_script_record *record;
};

//ベースタグリスト
struct base_tag_list {

	unsigned short BaseTagCount;
	unsigned char *BaselineTag;
};

//アキシズテーブル
struct axis_table {

	unsigned short BaseTagList;
	unsigned short BaseScriptList;
};

//ベースラインヘッダ
struct BASE_header {

	unsigned int Version;
	unsigned short HolizAxis;
	unsigned short VertAxis;
};

//------

//---GDEF---

struct GDEF_header {

	unsigned int Version;
	unsigned short GlyphClassDef;
	unsigned short AttachList;
	unsigned short LigCaretList;
	unsigned short MarkAttachClassDef;
};

//------

//---GPOS---

struct GPOS_header {

	unsigned int Version;
	unsigned short ScriptList;
	unsigned short FeatureList;
	unsigned short LookupList;
};

//------

//---OS/2---

struct OS2_data {

	unsigned short version;
	short xAvgCharWidth;
	unsigned short usWeightClass;
	unsigned short usWidthClass;
	unsigned short fsType;
	short ySubscriptXSize;
	short ySubscriptYSize;
	short ySubscriptXOffset;
	short ySubscriptYOffset;
	short ySuperscriptXSize;
	short ySuperscriptYSize;
	short ySuperscriptXOffset;
	short ySuperscriptYOffset;
	short yStrikeoutSize;
	short yStrikeoutPosition;
	short sFamilyClass;
	unsigned char panose[10];
	unsigned int ulUnicodeRange1;
	unsigned int ulUnicodeRange2;
	unsigned int ulUnicodeRange3;
	unsigned int ulUnicodeRange4;
	char achVendID[4];
	unsigned short fsSelection;
	unsigned short usFirstCharIndex;
	unsigned short usLastCharIndex;
	short sTypoAscender;
	short sTypoDescender;
	short sTypoLineGap;
	unsigned short usWinAscent;
	unsigned short usWinDescent;
	unsigned int CodePageRange1;
	unsigned int CodePageRange2;
	short sxHeight;
	short sCapHeight;
	unsigned short usDefaultChar;
	unsigned short usBreakChar;
	unsigned short usMaxContext;
};

//------


//---CMAP---

struct CMAP_header {

	unsigned short version;
	unsigned short numTables;
};

struct EncordTable {

	unsigned short PlatformID;
	unsigned short EncordingID;
	unsigned int offset;
};

struct Group {

	unsigned int startCharCode;
	unsigned int endCharCode;
	unsigned int startGlyphID;
};

struct Format12 {

	unsigned short format;
	unsigned short reserved;
	unsigned int length;
	unsigned int language;
	unsigned int nGroups;
};

//------


//---GASP---

struct GASP_Range {

	unsigned short rangeMaxPPEM;
	unsigned short rangeGaspBehavior;
};

struct GASP_header {

	unsigned short version;
	unsigned short numRanges;
	struct GASP_Range *gaspRange;
};

//------

//---GLYF---

#define FONT_GLYF_SIMPLE_ON_CURVE		0x01
#define FONT_GLYF_SIMPLE_X_SHORT_VECTOR	0x02
#define FONT_GLYF_SIMPLE_Y_SHORT_VECTOR	0x04
#define FONT_GLYF_SIMPLE_REPEAT			0x08
#define FONT_GLYF_SIMPLE_THIS_X_IS_SAME	0x10
#define FONT_GLYF_SIMPLE_THIS_Y_IS_SAME	0x20
#define FONT_GLYF_SIMPLE_POSITIVE_X_SHORT_VECTOR	0x10
#define FONT_GLYF_SIMPLE_POSITIVE_Y_SHORT_VECTOR	0x20

struct GLYF_header {

	short numberOfCntours;
	short xMin;
	short yMin;
	short xMax;
	short yMax;
};
//通常のグリフ
struct Simple_Glyph_Description {

	unsigned short *endPtsOfContours;
	unsigned short instructionLength;
	unsigned char *instructions;
	unsigned char flagLength;
	unsigned char *flags;
	short *xCoordinates;
	short *yCoordinates;
};

//------

//---HEAD---

struct HEAD_header {

	int version;
	int fontRevision;
	unsigned int checkSumAdjustment;
	unsigned int magicNumber;
	unsigned short flags;
	unsigned short unitsPerEm;
	long created;
	long modified;
	short xMin;
	short yMin;
	short xMax;
	short yMax;
	unsigned short macStyle;
	unsigned short lowestRecPPEM;
	short fontDirectionHint;
	short indexToLocFormat;
	short glyphDataFormat;
};

//------

//---HHEA---

struct HHEA_header {

	int version;
	short Ascender;
	short Descender;
	short LineGap;
	unsigned short advanceWidthMax;
	short minLeftSideBearing;
	short minRightSideBearing;
	short xMaxExtent;
	short caretSlopeRise;
	short caretSlopeRun;
	short caretOffset;
	short reserved[4];
	short metricDataFormat;
	unsigned short numberOHMetrics;
};

//------

//---MAXP---

struct MAXP_header {

	int version;
	unsigned short numGlyphs;
	unsigned short maxPoints;
	unsigned short maxContours;
	unsigned short maxCompositePoints;
	unsigned short maxCompositeCOntours;
	unsigned short maxZones;
	unsigned short maxTwilightPoints;
	unsigned short maxStorage;
	unsigned short maxFunctionDefs;
	unsigned short maxInstructionDefs;
	unsigned short maxStackElements;
	unsigned short maxSizeOfInstructions;
	unsigned short maxComponentElements;
	unsigned short maxComponentDepth;
};

//------

//---NAME---

struct NameRecord {

	unsigned short platformID;
	unsigned short encodingID;
	unsigned short languageID;
	unsigned short nameID;
	unsigned short length;
	unsigned short offset;
};

struct LangTagRecord {

	unsigned short length;
	unsigned short offset;
};

struct NAME_header {

	unsigned short format;
	unsigned short count;
	unsigned short stringOffsets;
	struct NameRecord *nameRecord;
	unsigned short langTagCount;
	struct LangTagRecord *langTagRecord;

};

//------

//---POST---

struct POST_header {

	int Version;
	int italicAngle;
	short underlinePosition;
	short underlineThickness;
	unsigned int isFixedPitch;
	unsigned int minMemType42;
	unsigned int maxMemType42;
	unsigned int minMemType1;
	unsigned int maxMemType1;
};

//------

//---VHEA---


struct VHEA_header {

	int version;
	short Ascender;
	short Descender;
	short LineGap;
	short advanceWidthMax;
	short minTopSideBearing;
	short minBottomSideBearing;
	short yMaxExtent;
	short caretSlopeRise;
	short caretSlopeRun;
	short caretOffset;
	short reserved[4];
	short metricDataFormat;
	unsigned short numberOfLongVerMetrics;
};

//------

struct font_info {

	struct table_directory *table;
	struct BASE_header *base;
	struct GDEF_header *gdef;
	struct GPOS_header *gpos;
	struct OS2_data *os2;
	struct CMAP_header *cmap;
	struct GASP_header *gasp;
	struct GLYF_header *glyf;
	struct HEAD_header *head;
	struct HHEA_header *hhea;
	struct MAXP_header *maxp;
	struct NAME_header *name;
	struct POST_header *post;
	struct VHEA_header *vhea;

	unsigned int *glyph_unicode;
};




//フォントファイル初期化
void font_file_init(char *path, struct font_info *info);

void load_glyf(struct font_info *info, struct Simple_Glyph_Description *glyph);

unsigned int get_glyphID(struct font_info *info, unsigned short c);

























