/*
 * keyboard.h
 *
 *  Created on: 2014/08/21
 *      Author: StarRing
 */

#pragma once


#include <system.h>

//キーボード ステータスレジスタのビット

//アウトプットバッファフル
#define KEYBOARD_STATUS_OBF		0x01
//インプットバッファフル
#define KEYBOARD_STATUS_IBF		0x02
//システムフラグ
#define KEYBOARD_STATUS_F0		0x04
//コマンド/データフラグ
#define KEYBOARD_STATUS_F1		0x08
//禁止フラグ
#define KEYBOARD_STATUS_ST4		0x10
//送信タイムアウトフラグ
#define KEYBOARD_STATUS_ST5		0x20
//受信タイムアウトフラグ
#define KEYBOARD_STATUS_ST6		0x40
//パリティフラグ
#define KEYBOARD_STATUS_ST7		0x80


//I/Oポート番号

//キーボードエンコーダ
#define KEYBOARD_PORT_ENCODER		0x60
//キーボードコントローラ
#define KEYBOARD_PORT_CONTROLLER	0x64

//キーボードエンコーダ コマンド
#define KEYBOARD_ENC_SETLED				0xED
#define KEYBOARD_ENC_ECHO				0xEE
#define KEYBOARD_ENC_GETSET_SCANCODE	0xF0
#define KEYBOARD_ENC_GET_KBD_ID			0xF2
#define KEYBOARD_ENC_SET_TYPEMATIC		0xF3
#define KEYBOARD_ENC_ENABLE_KBD			0xF4
#define KEYBOARD_ENC_DISABLE_KBD		0xF5
#define KEYBOARD_ENC_SET_DEFAULT		0xF6
#define KEYBOARD_ENC_SET_ALL_REPEAT		0xF7
#define KEYBOARD_ENC_SET_ALL_BREAK		0xF8
#define KEYBOARD_ENC_CLEAR_ALL_REP_BRK	0xF9
#define KEYBOARD_ENC_SET_ALL_REP_BRK	0xFA
#define KEYBOARD_ENC_SET_REPEAT			0xFB
#define KEYBOARD_ENC_SET_BREAK			0xFC
#define KEYBOARD_ENC_CLEAR_REP_BRK		0xFD
#define KEYBOARD_ENC_REQ_RESEND			0xFE
#define KEYBOARD_ENC_RESET_SELFTEST		0xFF

//キーボードコントローラコマンド
#define KEYBOARD_CTRL_COM_READ_CCB		0x20
#define KEYBOARD_CTRL_COM_WRITE_CCB		0x60
#define KEYBOARD_CTRL_COM_DISABLE_MOUSE	0xA7
#define KEYBOARD_CTRL_COM_ENABLE_MOUSE	0xA8
#define KEYBOARD_CTRL_COM_TEST_MOUSE		0xA9
#define KEYBOARD_CTRL_COM_SELF_TEST		0xAA
#define KEYBOARD_CTRL_COM_TEST_IF		0xAB
#define KEYBOARD_CTRL_COM_DISABLE_KBD	0xAD
#define KEYBOARD_CTRL_COM_ENABLE_KBD		0xAE
#define KEYBOARD_CTRL_COM_READ_IN_PORT	0xC0
#define KEYBOARD_CTRL_COM_READ_OUT_PORT	0xD0
#define KEYBOARD_CTRL_COM_WRITE_OUT_PORT	0xD1
#define KEYBOARD_CTRL_COM_WRITE_MOUSE	0xD4
#define KEYBOARD_CTRL_COM_READ_TEST_PORT	0xE0
#define KEYBOARD_CTRL_COM_SYSTEM_RESET	0xFE


//Shiftなどのサブキー スキャンコード
#define L_CTRL	0x1D
#define R_CTRL	0xE01D
#define L_ALT	0x38
#define R_ALT	0xE038
#define L_SHIFT	0x2A
#define R_SHIFT	0x36


//キーステート
struct key_state {

	bool num_lock;
	bool caps_lock;
	bool scr_lock;
	bool insert;

	bool L_shift;
	bool R_shift;
	bool L_ctrl;
	bool R_ctrl;
	bool L_alt;
	bool R_alt;

};



void keyboard_init(void);



//キーボードステータスを取得する
static __inline__ unsigned char read_status(void) {

	return inb(KEYBOARD_PORT_CONTROLLER);
}


//キーボードエンコーダのバッファを取得する
static __inline__ unsigned char read_buffer(void) {

	return inb(KEYBOARD_PORT_ENCODER);
}


















