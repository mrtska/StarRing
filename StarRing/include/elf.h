/*
[Module elf.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/27
*/

#pragma once


#include <fs/fs.h>
#include <task.h>
#include <system.h>

struct elf_ident {

	unsigned char magic1;
	unsigned char magic2;
	unsigned char magic3;
	unsigned char magic4;
	unsigned char class;
	unsigned char data;
	unsigned char version;
	unsigned char pad;
	unsigned char nident[8];


} __attribute__((packed));






struct elf32_header {

	struct elf_ident e_ident;	//

#define ET_NONE		0x0000
#define ET_REL		0x0001
#define ET_EXEC		0x0002
#define ET_DYN		0x0003
#define ET_CORE		0x0004
#define ET_LOPROC	0xFF00
#define ET_HIPROC	0xFFFF
	unsigned short e_type;

#define EM_NONE		0x0000
#define EM_M32		0x0001
#define EM_SPARC	0x0002
#define EM_386		0x0003
#define EM_68K		0x0004
#define EM_88K		0x0005
#define EM_860		0x0007
#define EM_MIPS		0x0008
	unsigned short e_machine;

	unsigned int e_version;
	unsigned int e_entry;
	unsigned int e_phoff;
	unsigned int e_shoff;
	unsigned int e_flags;
	unsigned short e_ehsize;
	unsigned short e_phentsize;
	unsigned short e_phnum;
	unsigned short e_shentsize;
	unsigned short e_shnum;
	unsigned short e_shstrndx;
};

struct elf64_header {

	struct elf_ident e_ident;	//IDエントリ
	unsigned short e_type;		//オブジェクトファイルタイプ
	unsigned short e_machine;	//アーキテクチャー
	unsigned int e_version;		//ELFのバージョン
	unsigned long e_entry;		//エントリポイント仮想アドレス
	unsigned long e_phoff;		//プログラムヘッダーテーブルのオフセット
	unsigned long e_shoff;		//セクションヘッダーテーブルのオフセット
	unsigned int e_flags;		//プロセッサー固有の情報
	unsigned short e_ehsize;	//ELFヘッダーのサイズ
	unsigned short e_phentsize;	//プログラムヘッダーテーブルの各エントリのサイズ
	unsigned short e_phnum;		//プログラムヘッダーテーブルの数
	unsigned short e_shentsize;	//セクションヘッダーテーブルの各エントリのサイズ
	unsigned short e_shnum;		//セクションヘッダーテーブルの数
	unsigned short e_shstrndx;	//セクション名文字列テーブルを指すインデックス値
};


struct elf64_program_header {

#define ELF_PT_NULL		0
#define ELF_PT_LOAD		1
#define ELF_PT_DYNAMIC	2
#define ELF_PT_INTERP	3
#define ELF_PT_NOTE		4
#define ELF_PT_SHLIB	5
#define ELF_PT_PHDR		6
#define ELF_PT_TLS		7
#define ELF_PT_LOPROC	0x70000000
#define ELF_PT_HIPROC	0x7FFFFFFF
	unsigned int p_type;		//セグメントタイプ

#define ELF_PF_X		0x1
#define ELF_PF_W		0x2
#define ELF_PF_R		0x4
	unsigned int p_flags;		//セグメントフラグ
	unsigned long p_offset;		//セグメントオフセット
	unsigned long p_vaddr;		//セグメントがある仮想アドレス
	unsigned long p_paddr;		//予約
	unsigned long p_fizesz;		//セグメントのファイルイメージサイズ
	unsigned long p_memsz;		//メモリ上に配置されるセグメントのサイズ
	unsigned long p_align;		//セグメントのアライメント
};


struct elf64_rela {

	unsigned long r_offset;

#define ELF64_R_SYM(i)((i) >> 32)
#define ELF64_R_TYPE(i)((i) & 0xFFFFFFFFL)
#define ELF64_R_INFO(s, t)(((s) << 32) + ((t) & 0xFFFFFFFFL))
	unsigned long r_info;
	signed long r_addend;
};

//ELF ダイナミックセクション
struct elf64_dyn {

#define DT_NULL				0
#define DT_NEEDED			1
#define DT_PLTRELSZ			2
#define DT_PLTGOT			3
#define DT_HASH				4
#define DT_STRTAB			5
#define DT_SYMTAB			6
#define DT_RELA				7
#define DT_RELASZ			8
#define DT_RELAENT			9
#define DT_STRSZ			10
#define DT_SYMENT			11
#define DT_INIT				12
#define DT_FINI				13
#define DT_SONAME			14
#define DT_RPATH			15
#define DT_SYMBOLIC			16
#define DT_REL				17
#define DT_RELSZ			18
#define DT_RELENT			19
#define DT_PLTREL			20
#define DT_DEBUG			21
#define DT_TEXTREL			22
#define DT_JMPREL			23
#define DT_BIND_NOW			24
#define DT_INIT_ARRAY		25
#define DT_FINI_ARRAY		26
#define DT_INIT_ARRAY_SZ	27
#define DT_FINI_ARRAY_SZ	28
	signed long d_tag;

	union {

		unsigned long d_val;
		void *d_ptr;
	} d_un;
};



struct elf64_dynamic_table {

	unsigned long index;
	unsigned long pltrel_size;
	unsigned long *pltgot;
	unsigned long *hash;
	char *strtab;
	char *symtab;
	unsigned long *rela;
	unsigned long rela_size;
	unsigned long rela_entry;
	unsigned long str_size;
	unsigned long sym_entry;
	unsigned long *init_addr;
	unsigned long *fini_addr;
	unsigned long so_name;
	unsigned long rpath;

	unsigned long *rel;
	unsigned long rel_size;
	unsigned long rel_entry;
	unsigned long plt_rel;
	unsigned long *debug;

	unsigned long *jmp_rel;

	char *name;
	bool is_rela;



};




struct elf64_section_header {

	unsigned int sh_name; /* Section name */

#define ELF_SHT_NULL		0	//未使用のセクション
#define ELF_SHT_PROGBITS	1	//通常のセクション
#define ELF_SHT_SYMTAB		2	//リンカーシンボルテーブル
#define ELF_SHT_STRTAB		3	//文字列テーブル
#define ELF_SHT_RELA		4	//Relaタイプ再配置エントリ
#define ELF_SHT_HASH		5	//シンボルハッシュテーブル
#define ELF_SHT_DYNAMIC		6	//ダイナミックリンクテーブル
#define ELF_SHT_NOTE		7	//ノート情報
#define ELF_SHT_NOBITS		8	//BSSセクション
#define ELF_SHT_REL			9	//Relタイプ再配置エントリ
#define ELF_SHT_SHLIB		10	//予約
#define ELF_SHT_DYNSYM		11	//ダイナミックローダーシンボルテーブル
#define ELF_SHT_NUM			12
#define ELF_SHT_LOPROC		0x70000000	//プロセッサ固有情報 下位
#define ELF_SHT_HIPROC		0x7FFFFFFF	//プロセッサ固有情報 上位
#define ELF_SHT_LOUSER		0x80000000
#define ELF_SHT_HIUSER		0xFFFFFFFF
	unsigned int sh_type; /* Section type */

#define ELF_SHF_WRITE		0x1
#define ELF_SHF_ALLOC		0x2
#define ELF_SHF_EXECINSTR	0x4
#define ELF_SHF_MASKOS		0x0F000000
#define ELF_SHF_MASKPROC	0xF0000000
	unsigned long sh_flags; //セクションの属性
	unsigned long sh_addr; /* Virtual address in memory */
	unsigned long sh_offset; /* Offset in file */
	unsigned long sh_size; /* Size of section */
	unsigned int sh_link; /* Link to other section */
	unsigned int sh_info; /* Miscellaneous information */
	unsigned long sh_addralign; /* Address alignment boundary */
	unsigned long sh_entsize; /* Size of entries, if section has table */
};
/* st_infoから情報を抽出する定義 */
#define ELF32_ST_BIND(i)	((i) >> 4)
#define ELF32_ST_TYPE(i)	((i) & 0x0F)
/* st_infoに情報を設定する定義 b=ST_BIND, t=ST_TYPE */
#define ELF32_ST_INFO(b, t)	(((b) << 4)) + ((t) & 0x0F))
struct elf64_sym {

	unsigned int st_name;
	unsigned char st_info;
	unsigned char st_other;
	unsigned short st_shndx;
	unsigned long st_value;
	unsigned long st_size;
} __attribute((packed));

#include <mm.h>

#define elf_write64(p, v) (*(unsigned long*)p = v)


#define round(size, align) (((size) + (align) - 1) & ~((align) - 1))
#define calculate_first_tls_offset(size, align) round(size, align)


int load_elf_binary(struct process *process);

int load_elf64_shared_library(struct process *process, struct elf64_dynamic_table *table);





void elf64_create_table(struct process *process, void *p);
























