/*
[Module vbox.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/09/08
*/

#pragma once


//R = Read, W = Write, I = Index, D = Data, RO = Read Only


//ステータスレジスタ RO
#define VBOX_STATUS_0_R 0x3C2
#define VBOX_STATUS_1_R 0x3DA


//Miscellaneousレジスタ R/W
#define VBOX_MISC_R 0x3CC
#define VBOX_MISC_W 0x3C2

//シーケンサーレジスタ I/D
#define VBOX_SEQUENCER_I 0x3C4
#define VBOX_SEQUENCER_D 0x3C5

//アトリビュートレジスタ I/D
#define VBOX_ATTRIBUTE_I 0x3C0
#define VBOX_ATTRIBUTE_D 0x3C1

//CRTコントロールレジスタ I/D
#define VBOX_CRTCTRL_I 0x3D4
#define VBOX_CRTCTRL_D 0x3D5

//ディスプレイコントロールレジスタ I/D
#define VBOX_DISPLAY_I 0x1CE
#define VBOX_DISPLAY_D 0x1CF

//グラフィックスレジスタ I/D
#define VBOX_GRAPHICS_I 0x3CE
#define VBOX_GRAPHICS_D 0x3CF





#define VBOX_GRAPHICS_ADAPTER_VENDOR_ID 0x80EE
#define VBOX_GRAPHICS_ADAPTER_DEVICE_ID 0xBEEF




extern unsigned long vram;

void vbox_graphics_init(unsigned int x, unsigned int y);
























