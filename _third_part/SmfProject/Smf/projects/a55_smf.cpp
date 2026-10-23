/***************************************************************************
 *
 * Copyright 2015-2025 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <stdio.h>
#include "smf_api.h"
#include "smf_debug.h"
#include "hal_location.h"
#include "smf_msg.h"
#include "smf_api_ics.h"
#include "smf_api_proxy.h"
#include "thread.h"
#include "Semaphore.h"
#include "hal_trace.h"
#include "open-amp/rptun.h"
#include "best1700/hal_maincmu_best1700.h"
#include "SmfPorting.h"
//

using namespace smf;

static smf::semaphore _sem;
#define SMF_GLOBLE_POOL_SIZE (1024*1024*1)
#define SMF_NC_POOL_SIZE (1024*1024*80)
//static PSRAM_NC_BSS_LOC int _psram_nc_data[SMF_NC_POOL_SIZE / 4];
static uint8_t _psram_nc_data[SMF_NC_POOL_SIZE] locate_data(".sync_flags") __attribute__((aligned(64)));
static bool msg_service_opened_flag = false;
EXTERNC void smf_project_memory(void* buffer, uint32_t size) {
	uint32_t ics_buffer;
	uint32_t ics_buflen;
	dbgInfoPXL("try get ics buffer");
	do {
		int ret = rptun_get_ics_buffer(RPTUN_CHAN_M55C0_2_CASYS, &ics_buffer, &ics_buflen);
		if (ret || !ics_buffer || !ics_buflen) {
			// dbgErrPXL("get ics buffer failed:%d", ret);
			sleep_for(10);
		} else {
			break;
		}
	}while(1);
	dbgInfoPXL("ics:0x%08x,0x%08x", ics_buffer, ics_buflen);
	smf_register_ics((void*)ics_buffer, ics_buflen, false);
	smf_register_pool("gbl", buffer, size, 1024 * 4);
	smf_register_pool("nc", _psram_nc_data, sizeof(_psram_nc_data), 1024 * 128);
	smf_designate_globle_pool("gbl");
	dbgTestPXL("pool:%p,size:%d",buffer, size);
#if 0
	extern uint32_t __json_file_start[];
	extern uint32_t __json_file_end[];
	smf_register_script((void*)__json_file_start, __json_file_end - __json_file_start);
	dbgTestPXL("__json_file_start:%p,",__json_file_start);
#endif
}

//
EXTERNC void smf_project_register() {
	//
	SMF_REGISTER(fs);
	//
	SMF_REGISTER(background);
	SMF_REGISTER(notify_bg);
	//
	SMF_REGISTER(pipeline);
	SMF_REGISTER(muxer);
	//SMF_REGISTER(demuxer);
	SMF_REGISTER(decoder);
	SMF_REGISTER(encoder);
	SMF_REGISTER(io_source);
	SMF_REGISTER(io_sink);
	// SMF_REREGISTER("mux-h264", "sink-io");
	SMF_REGISTER(func_source);
	SMF_REGISTER(func_sink);
	SMF_REGISTER(callback_source);
	SMF_REGISTER(callback_sink);
	SMF_REGISTER(dumy_source);
	SMF_REGISTER(dumy_sink);

	//stream server
	SMF_REGISTER(stream_server);
	SMF_REGISTER(stream_client);
	//media manager
	SMF_REGISTER(media_manager);

	//msg service
	SMF_REGISTER(msg_service);
	SMF_REGISTER(msg_client_core);
	//SMF_REGISTER(msg_client_fs);
	SMF_REGISTER(msg_client_pool1_remote);
	// SMF_REGISTER(msg_client_pool1);
	SMF_REGISTER(msg_client_hook_client);

	//communication element
	SMF_REGISTER(shm_source);
	SMF_REGISTER(shm_sink);	

	SMF_REGISTER(isp_source);
	SMF_REGISTER(isp_sink);
	SMF_REGISTER(isp_service);

	SMF_REGISTER(imu_source);
	SMF_REGISTER(imu_service);

	SMF_REGISTER(yuv_encoder);
	SMF_REGISTER(h264_encoder);
	SMF_REGISTER(jpeg_encoder);
	SMF_REGISTER(h265_encoder);

	SMF_REGISTER(h264_decoder);
	SMF_REGISTER(jpeg_decoder);
	SMF_REGISTER(h265_decoder);
}

EXTERNC void smf_project_command() {
	// SMF_CMD_REGISTER(isp);
	SMF_CMD_REGISTER(mmgr);
	SMF_CMD_REGISTER(camera);
}

EXTERNC void smf_project_apps() {
	SMF_APP_REGISTER(media);
	//SMF_APP_REGISTER(audio_player);
	//SMF_APP_REGISTER(audio_record);
	//SMF_APP_REGISTER(video_player);
	//SMF_APP_REGISTER(video_record);
	//SMF_APP_REGISTER(av_player);
	//SMF_APP_REGISTER(av_record);
	//SMF_APP_REGISTER(audio_sco);
	//SMF_APP_REGISTER(audio_vad);
	//SMF_APP_REGISTER(audio_a2dp);
	//SMF_APP_REREGISTER("imu_service","app_media");
	SMF_APP_REGISTER(camera_service);
	SMF_APP_REGISTER(camera_stream);
}

EXTERNC bool smf_msg_service_open(void* buffer, uint32_t size) {
	smf_init();
	returnIfErrC(false, !buffer || !size);
	smf_set_cpu(SMF_CPU_CP_4, "casys");
	smf_project_memory(buffer, size);
	smf_project_register();
	smf_project_command();
	smf_project_apps();
#if 1
	returnIfErrC(false, !smf_ics_sync_ready(SMF_CPU_AP, 1000, 5));
	returnIfErrC(false, !smf_config_from_ics());
#else
	extern uint32_t __json_file_start[];
	dbgTestPXL("__json_file_start:%p,",__json_file_start);
	returnIfErrC(false, !smf_config_from_json((const char*)__json_file_start));
#endif
	smf_ics_ready();
	return true;
}

EXTERNC bool smf_msg_service_close() {
	smf_uninit();
	return true;
}

EXTERNC bool smf_msg_service_start() {
	return true;
}

EXTERNC bool smf_msg_service_stop() {
	return true;
}

static void* gbl = NULL;
EXTERNC void* smf_service_init(void*){
	if(gbl){
		TRACE(0,"[%s][%d] gbl already exists",__func__,__LINE__);
		return NULL;
	}
	gbl = malloc(SMF_GLOBLE_POOL_SIZE);
	if(gbl){
		smf_msg_service_open(gbl, SMF_GLOBLE_POOL_SIZE);
	}else{
		TRACE(0,"[%s][%d] malloc failed",__func__,__LINE__);
	}
	return NULL;
}
EXTERNC void smf_service_deinit(){
	smf_msg_service_close();
	if(gbl)free(gbl);
	gbl = NULL;
}

static void smf_service_open(){
	if(msg_service_opened_flag)return;
	msg_service_opened_flag = true;
	pthread_t pid;
    int ret = pthread_create(&pid, 0, smf_service_init, NULL);
	if(ret){
		TRACE(0,"smf_service_open thread create failed errid=%d",ret);
		return;
	}
	_sem.wait();
}

static void smf_service_close(){
	smf_service_deinit();
	_sem.post();
	msg_service_opened_flag = false;
}

EXTERNC void smf_enter_a7(int argc, char* argv[]) {
	if(strcmp(argv[1], "open") == 0){
		TRACE(0,"smf_service_open \n");
		smf_service_open();
	}else if(strcmp(argv[1], "close") == 0){
		TRACE(0,"smf_service_close \n");
		smf_service_close();
	}else if(strcmp(argv[1], "init") == 0){
		TRACE(0,"smf_service_init \n");
		smf_service_init(NULL);
	}else if(strcmp(argv[1], "deinit") == 0){
		TRACE(0,"smf_service_deinit \n");
		smf_service_deinit();
	}
	else {
		smf_cmd_entry(argc, argv);
	}
}
