/*
 * page_types.h
 *
 *  Created on: 2014/04/19
 *      Author: StarRing
 */

#pragma once



//テーブルエントリ
typedef union _table_entry {

	struct {
		unsigned p:1;				//Present
		unsigned rw:1;				//Read/Write
		unsigned us:1;				//User/Supervisor
		unsigned pwt:1;				//Page Level Write Throgh
		unsigned pcd:1;				//Page Level Cache Disable
		unsigned a:1;				//Access
		unsigned d:1;				//Dirty
		unsigned pat:1;				//Page Attribute Table
		unsigned g:1;				//Global
		unsigned Ignore:3;			//Ignore
		unsigned long address:40;	//Frame Address
		unsigned ignore2:11;		//Ignore
		unsigned exd:1;				//Excutable Disable
	};
	unsigned long data;

} __attribute__((packed)) pte_t;

typedef union table_entry {

	struct {
		unsigned p:1;				//Present
		unsigned rw:1;				//Read/Write
		unsigned us:1;				//User/Supervisor
		unsigned pwt:1;				//Page Level Write Throgh
		unsigned pcd:1;				//Page Level Cache Disable
		unsigned a:1;				//Access
		unsigned Ignore1:1;			//Ignore
		unsigned zero:1;			//Must be zero | Large Page
		unsigned Ignore2:4;			//Ignore
		unsigned long address:40;	//Frame Address
		unsigned ignore2:11;		//Ignore
		unsigned exd:1;				//Excutable Disable
	};
	unsigned long data;

} __attribute__((packed)) pde_t;

typedef union table_entry pdpte_t;
typedef union table_entry pml4_t;









