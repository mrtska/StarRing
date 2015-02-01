/*
[Module ata.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
 *  Created on: 2014/10/09
*/

#pragma once


//ATA I/Oポート
#define ATA_DATA_R			0x1F0	// R/W
#define ATA_ERROR_R			0x1F1	// R
#define ATA_FEATURES_R		0x1F1	// W
#define ATA_SECTOR_COUNT_R	0x1F2	// R/W
#define ATA_LBA_LOW_R		0x1F3	// R/W
#define ATA_LBA_MID_R		0x1F4	// R/W
#define ATA_LBA_HIGH_R		0x1F5	// R/W
#define ATA_DEVICE_HEAD_R	0x1F6	// R/W
#define ATA_STATUS_R		0x1F7	// R
#define ATA_COMMAND_R		0x1F7	// W
#define ATA_AL_STATUS_R		0x3F6	// R
#define ATA_DEVICE_CTRL_R	0x3F6	// W


//ATA STATUS
#define ATA_STATUS_ERROR	0x01
#define ATA_STATUS_IDX		0x02
#define ATA_STATUS_CORR		0x04
#define ATA_STATUS_DRQ		0x08
#define ATA_STATUS_DSC		0x10
#define ATA_STATUS_DF		0x20
#define ATA_STATUS_DRDY		0x40
#define ATA_STATUS_BUSY		0x80


//ATA CMD
#define ATA_CMD_READ_PIO			0x20
#define ATA_CMD_READ_PIO_EXT		0x24
#define ATA_CMD_READ_DMA			0xC8
#define ATA_CMD_READ_DMA_EXT		0x25
#define ATA_CMD_WRITE_PIO			0x30
#define ATA_CMD_WRITE_PIO_EXT		0x34
#define ATA_CMD_WRITE_DMA			0xCA
#define ATA_CMD_WRITE_DMA_EXT		0x35
#define ATA_CMD_CACHE_FLUSH			0xE7
#define ATA_CMD_CACHE_FLUSH_EXT		0xEA
#define ATA_CMD_PACKET				0xA0
#define ATA_CMD_IDENTIFY_PACKET		0xA1
#define ATA_CMD_IDENTIFY			0xEC

#define ATA_CMD_SET_FEATURES		0xEF

#define ATA_CMD_INIT_DEVICE_PARAMETER	0x91


//ATA デバイス情報
struct ata_identify {

	struct {

		unsigned reserved1 : 1;
		unsigned obsolete1 : 1;
		unsigned incomplete_responce : 1;
		unsigned obsolete2 : 3;
		unsigned non_removable : 1;
		unsigned removable : 1;
		unsigned obsolete3 : 7;
		unsigned zero : 1;

	} __attribute__((packed)) flags0;



	unsigned short cylinders;
	unsigned short special_conf;
	unsigned short heads;
	unsigned short obsolete1[2];
	unsigned short sectors;
	unsigned short reserved1[2];
	unsigned short obsolete2;
	char serial[20];
	unsigned short obsolete3[2];
	unsigned short obsolete4;
	char firmware[8];
	char model[40];
	unsigned char interrupt_per_secotrs;
	unsigned char val0x80;
	unsigned short reserved2;

	struct {

		unsigned char obsolete1;
		unsigned one1 : 1;
		unsigned one2 : 1;
		unsigned disable_iordy : 1;
		unsigned support_iordy : 1;
		unsigned reserved1 : 1;
		unsigned standard_standby_timer : 1;
		unsigned reserved2 : 2;
	} __attribute__((packed)) flags49;

	struct {

		unsigned standby_timer_minval : 1;
		unsigned obsolete1 : 1;
		unsigned reserved1 : 12;
		unsigned one1 : 1;
		unsigned zero1 : 1;
	} __attribute__((packed)) flags50;

	unsigned short obsolete5[2];

	struct {

		unsigned enable_word54_58 : 1;
		unsigned enable_word64_70 : 1;
		unsigned enable_word80 : 1;
		unsigned reserved1 : 13;
	} __attribute__((packed)) flags53;

	unsigned short current_cylinders;
	unsigned short current_heads;
	unsigned short current_sectors;
	unsigned int current_all_sectors;

	struct {

		unsigned char interrupt_per_sectors;
		unsigned enable_set_multiple : 1;
		unsigned reserved1 : 7;
	} __attribute__((packed)) flags59;

	unsigned int sectors_28;
	unsigned short obsolete6;

	struct {

		unsigned support_mDMA0 : 1;
		unsigned support_mDMA1 : 1;
		unsigned support_mDMA2 : 1;
		unsigned zero1 : 5;
		unsigned choose_mDMA0 : 1;
		unsigned choose_mDMA1 : 1;
		unsigned choose_mDMA2 : 1;
		unsigned reserved1 : 5;
	} __attribute__((packed)) flags63;

	unsigned char support_pio_mode;
	unsigned char reserved3;

	unsigned short mDMA_min_cycle;
	unsigned short mDMA_recommend_cycle;
	unsigned short pio_min_cycle_non_fllow;
	unsigned short pio_min_cycle_fllow;

	unsigned int reserved4;

	unsigned long reserved_for_packet;

	unsigned char max_queue_size;
	unsigned char reserved5;

	unsigned long reserved_for_serial_ATA;

	unsigned short major_version;
	unsigned short minor_version;

	struct {

		unsigned smart_feature : 1;
		unsigned security_mode_feature : 1;
		unsigned removable_media_feature : 1;
		unsigned power_management_feature : 1;
		unsigned packet_command_feature : 1;
		unsigned write_cache : 1;
		unsigned locK_ahead : 1;
		unsigned release_interrupt : 1;
		unsigned service_interrupt : 1;
		unsigned device_rest_command : 1;
		unsigned host_protected_areafeature : 1;
		unsigned obsolete1 : 1;
		unsigned write_buffer_commnad : 1;
		unsigned read_buffer_command : 1;
		unsigned nop_command : 1;
		unsigned obsolete2 : 1;
	} __attribute__((packed)) support_command_set1;

	struct {

		unsigned download_microcode_commnad : 1;
		unsigned rw_DMA_queued_command : 1;
		unsigned cfa_feature : 1;
		unsigned advanced_power_management_feature : 1;
		unsigned removable_media_status_notification_feature : 1;
		unsigned power_up_in_standby_feature : 1;
		unsigned set_features_command_required_spinup : 1;
		unsigned reserved1 : 1;
		unsigned set_max_security_extention : 1;
		unsigned automatic_management_feature : 1;
		unsigned lba_48bit : 1;
		unsigned device_configuration_overlay_feature : 1;
		unsigned force_flush_cache_feature : 1;
		unsigned flush_chche_ext_feature : 1;
		unsigned one1 : 1;
		unsigned zero1 : 1;
	} __attribute__((packed)) support_command_set2;

	unsigned short exteneded_command_set_support;
	unsigned short command_set_enable1;
	unsigned short command_set_enable2;
	unsigned short command_set_extended_enable;

	union {

		unsigned short uDMA_union;

		struct {

			unsigned uDMA0 : 1;
			unsigned uDMA1 : 1;
			unsigned uDMA2 : 1;
			unsigned uDMA3 : 1;
			unsigned uDMA4 : 1;
			unsigned uDMA5 : 1;
			unsigned uDMA6 : 1;
			unsigned reserved1 : 1;
			unsigned choose_uDMA0 : 1;
			unsigned choose_uDMA1 : 1;
			unsigned choose_uDMA2 : 1;
			unsigned choose_uDMA3 : 1;
			unsigned choose_uDMA4 : 1;
			unsigned choose_uDMA5 : 1;
			unsigned choose_uDMA6 : 1;
			unsigned reserved2 : 1;
		} uDMA_field;

	} uDMA_support;

	unsigned short time_of_security_erase_unit;
	unsigned short time_of_enhanced_security_erace;
	unsigned short current_power_management_value;
	unsigned short master_password_revison;
	unsigned short result_of_hardware_reset;
	unsigned short vendor_difinition1;
	unsigned short stream_min_request_size;
	unsigned short time_of_streaming_dma;
	unsigned short streaming_access_latency;
	unsigned int streaming_performance_granularity;
	unsigned long sectors_48;
	unsigned short time_of_streaming_pio;
	unsigned short reserved6;
	unsigned short sector_size;
	unsigned short internal_seek_delay;
	unsigned long ieee_unique_id;
	unsigned long reserved7;
	unsigned short reserved8;
	unsigned int sector_per_word;
	unsigned short reserved9[8];

	struct {

		unsigned is_support : 2;
		unsigned reserved1 : 14;
	} __attribute__((packed)) flags127;

	unsigned short security_status;
	unsigned short vendor_difinition[31];
	unsigned short cfa_power_mode;
	unsigned short reserved_for_cfa[15];
	unsigned short current_media_serial_number[30];
	unsigned short reserved10[49];
	unsigned char sigunature;
	unsigned char checksum;

} __attribute__((packed));

//コマンドパラメーター
struct ata_param {

	unsigned char features;
	unsigned char sector_count;
	unsigned char sector_number;	//LBA7~0ビット
	unsigned char cylinder_low;		//LBA15~8ビット
	unsigned char cylinder_high;	//LBA23~16ビット
	unsigned char device_head;
	unsigned char command;

	unsigned char *buf;
};








