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
#include <assert.h>
#include "smf_api.h"
#include "smf_debug.h"
#include "hal_location.h"
#include "smf_msg.h"
#include "smf_api_ics.h"
#include "smf_api_pool.h"
#include "smf_api_proxy.h"
#include "thread.h"
#include "Semaphore.h"
#include "hal_trace.h"
#include "smf_msg.h"
#include "open-amp/rptun.h"
#include "SmfPortingFs.h"
#include "SmfPortingUser.h"
#include "hal_sysfreq.h"

using namespace smf;

typedef void (*CBSend)(const char* str, int size);
static bool _wait = false;

#define PSRAM_NC_SIZE (1024*1024*4)
#define SMF_GLOBLE_POOL_SIZE (1024*1024*1)
static PSRAM_NC_BSS_LOC int _psram_nc_data[PSRAM_NC_SIZE/4];
//static PSRAM_NC_BSS_LOC int64_t _json_data[40*1024/8];
static bool msg_service_opened_flag = false;

EXTERNC void smf_project_memory(void* buffer, uint32_t size) {
	uint32_t ics_buffer;
	uint32_t ics_buflen;
	int ret = rptun_get_ics_buffer(RPTUN_CHAN_M55C0_2_DSPC0, &ics_buffer, &ics_buflen);
	if (ret || !ics_buffer || !ics_buflen) {
		dbgErrPXL("get ics buffer failed");
		return;
	}
	dbgInfoPXL("ics:0x%08x,0x%08x", ics_buffer, ics_buflen);
	smf_register_ics((void*)ics_buffer, ics_buflen, true);
	smf_register_pool("gbl", buffer, size, 1024 * 32);
	smf_register_pool("nc", _psram_nc_data, PSRAM_NC_SIZE, 1024 * 64);
	dbgInfoPXL("psram nc:0x%08x,0x%08x", _psram_nc_data, PSRAM_NC_SIZE);
	smf_designate_globle_pool("gbl");
	dbgTestPXL("pool:%p,size:%d",buffer, size);
	// smf_register_script((void*)SMF_SCRIPT_ADDR, SMF_SCRIPT_SIZE);
	extern uint8_t __json_file_start[];
	extern uint8_t __json_file_end[];
	auto _json_size = __json_file_end - __json_file_start;
	// auto _json_data = smf_find_pool_alloc("nc", _json_size);
	// returnIfErrC0(!_json_data);
	//assert(sizeof(_json_data) < __json_file_end - __json_file_start);
	// memcpy(_json_data, __json_file_start, __json_file_end - __json_file_start);
	// smf_register_script((void*)_json_data, __json_file_end - __json_file_start);//AP cannot access the flash
	smf_register_script((void*)__json_file_start, __json_file_end - __json_file_start);

	// dbgTestPXL("__json_file_start:%p->%p,",__json_file_start, _json_data);
	dbgTestPXL("__json_file_start:%p",__json_file_start);
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
	SMF_REGISTER(demuxer);
	SMF_REGISTER(decoder);
	SMF_REGISTER(encoder);
	SMF_REGISTER(io_sink);
	SMF_REGISTER(io_source);
	SMF_REGISTER(callback_source);
	SMF_REGISTER(callback_sink);
	SMF_REGISTER(func_source);
	SMF_REGISTER(func_sink);
	//
	SMF_REGISTER(shm_source);
	SMF_REGISTER(shm_sink);
	//
	//SMF_REGISTER(log_service);
	//
	SMF_REGISTER(mp3_demuxer);
	SMF_REGISTER(wav_demuxer );
	SMF_REGISTER(pcm_demuxer);
	// SMF_REGISTER(aac_demuxer);
	//
	SMF_REGISTER(wav_muxer);
	SMF_REGISTER(mp3_muxer);
	SMF_REGISTER(bypass_muxer);
	// SMF_REGISTER(opus_muxer);

	SMF_REGISTER(msg_service);
	SMF_REGISTER(msg_client_core);
	//SMF_REGISTER(msg_client_audio_player_local);
	//SMF_REGISTER(msg_client_audio_record_local);
	//SMF_REGISTER(msg_client_avrecord_local);
	// SMF_REGISTER(msg_client_device_local);

	//SMF_REGISTER(msg_client_hook_local);
	SMF_REGISTER(msg_client_pool_remote);
	SMF_REGISTER(msg_client_pool1);
	// SMF_REGISTER(msg_client_sync);
	// SMF_REGISTER(msg_client_device_remote);
	SMF_REGISTER(msg_client_hook_client);
	SMF_REGISTER(msg_client_hook_server);
	SMF_REGISTER(io_file_pos);
	SMF_REGISTER(fifo_filter);
	SMF_REGISTER(frame_buffer);
	SMF_REGISTER(msg_client_fs_remote);
	SMF_REGISTER(libs_sink);
	SMF_REGISTER(libs_filter);
	// SMF_REGISTER(msg_client_vad);
	// SMF_REGISTER(io_sink);
	SMF_REGISTER(libs_filter);
	SMF_REGISTER(media_manager);

	//video
	SMF_REGISTER(mp4_muxer);
	SMF_REGISTER(mp4_muxer_video_adapt);
	SMF_REGISTER(mp4_demuxer);
	// SMF_REGISTER(mjpeg_demuxer);
	// SMF_REGISTER(h264_demuxer);
	// SMF_REGISTER(gif_demuxer);
	// SMF_REGISTER(m4a_demuxer);
	// SMF_REGISTER(m4v_demuxer);
	//
	// SMF_REGISTER(rtsp_service);
	// SMF_REGISTER(rtsp_player);
	//
	SMF_REGISTER(stream_client);
	SMF_REGISTER(stream_server_all);

	SMF_REGISTER(display_service);
	SMF_REGISTER(tswait_filter);
	SMF_REGISTER(display);
	//
	SMF_REGISTER(isp_source);
	SMF_REGISTER(isp_sink);
	SMF_REGISTER(isp_service);

#ifdef SMF_HAS_DYNLIB
	SMF_REGISTER(dynamic_loader);
#endif

	//net
#ifdef HTTP_MEDIA_EN
	SMF_REGISTER(io_http);
	SMF_REGISTER(io_https);
#endif
	//sco intersys
#ifdef SCO_OVER_INTERSYS
	SMF_REGISTER(sco_service);
#endif
	SMF_REGISTER(h264_muxer);
	// SMF_REREGISTER("mux-h264","sink-io");
}

EXTERNC void smf_project_command() {
	//mmgr
	SMF_CMD_REGISTER(mmgr);
	SMF_CMD_REGISTER(camera);
	//
	// SMF_CMD_REGISTER(stream);
	// SMF_CMD_REGISTER(stream_speaker);
	// SMF_CMD_REGISTER(stream_aplay);
	// SMF_CMD_REGISTER(stream_mic);
	// SMF_CMD_REGISTER(stream_arecord);
	// SMF_CMD_REGISTER(stream_a2dp);
	// SMF_CMD_REGISTER(stream_btsco);
	// SMF_CMD_REGISTER(stream_ims);
	// SMF_CMD_REGISTER(stream_pause);
	// SMF_CMD_REGISTER(stream_rtsp_server);
	// SMF_CMD_REGISTER(stream_rtsp_client);
	// SMF_CMD_REGISTER(stream_avplay);
}

EXTERNC void smf_project_apps() {
	SMF_APP_REGISTER(media);
	SMF_APP_REGISTER(audio_player);
	SMF_APP_REGISTER(audio_record);
	SMF_APP_REGISTER(video_player);
	SMF_APP_REGISTER(video_record);
	SMF_APP_REGISTER(av_player);
	SMF_APP_REGISTER(av_record);
	SMF_APP_REGISTER(audio_sco);
	SMF_APP_REGISTER(audio_vad);
	SMF_APP_REGISTER(audio_a2dp_ext);
	SMF_APP_REGISTER(camera_service);
	SMF_APP_REGISTER(camera_stream);
}

EXTERNC bool smf_msg_service_open(void* buffer, uint32_t size) {
	smf_init();
	//smf_log_redirect2((CBSend)cbTrace);
	returnIfErrC(false, !buffer || !size);
	smf_set_cpu(SMF_CPU_AP, "m55c0");
	smf_project_memory(buffer, size);
	smf_project_register();
	smf_project_command();
	smf_project_apps();

	returnIfErrC(false, !smf_config_from_ics());
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

static void* smf_service_init(void*){
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
static void smf_service_deinit(){
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
	_wait = true;
	while (_wait)smf_sleep_for(1000);
}

static void smf_service_close(){
	smf_service_deinit();
	_wait = false;
	msg_service_opened_flag = false;
}

EXTERNC void smf_msg_service_open_test() {
	TRACE(0,"smf_service_init %d", __LINE__);
	smf_service_init(NULL);
}

EXTERNC void smf_msg_service_close_test() {
	TRACE(0,"smf_service_deinit %d", __LINE__);
	smf_service_deinit();
}

EXTERNC void smf_enter_ap(int argc, char* argv[]) {
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
	}else {
		smf_cmd_entry(argc, argv);
	}
}
EXTERNC void smf_enter_ap_init() {

}
EXTERNC void smf_enter_ap_deinit() {

}

/* Customer function for PA control */
void smf_device_notify(smf_media_device_e dev, smf_media_device_state_e state) {
	dbgTestPXL("dev=%d state=%d", (int) dev, state);
	hal_subsysfreq_req(HAL_CMU_CP_SUBSYS_ID_DSP, HAL_CMU_FREQ_192M);
}