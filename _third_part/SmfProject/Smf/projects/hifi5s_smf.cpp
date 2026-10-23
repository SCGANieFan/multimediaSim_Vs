#include <stdio.h>
#include "smf_api.h"
#include "smf_debug.h"
#include "smf_api_ics.h"
#include "thread.h"
#include <stdlib.h>
#include "plat_addr_map.h"
#include <open-amp/rptun.h>
#ifdef USE_BES_RPCHAN
#include "open-amp/rpchan/bes_rpchan.h"
#endif
#include "hal_location.h"
//
typedef bool (*rpmsg_boot_cb_t)(const char *name);

//psram
#if 1
extern char _dpsram_heap_start[];
extern char _dpsram_heap_end[];
#endif
//dtcm
#ifndef HIFI4_DTCM_SIZE
#define HIFI4_DTCM_SIZE SYS_HIFI4C0_DTCM_SIZE
#endif
//dtcm
#ifndef HIFI4_ITCM_SIZE
#define HIFI4_ITCM_SIZE SYS_HIFI4C0_ITCM_SIZE
#endif
//system
EXTERNC size_t xPortGetFreeHeapSize(void);
EXTERNC size_t xPortGetTotalHeapSize(void);
EXTERNC void* pvPortMalloc(size_t xSize);
EXTERNC void vPortFree(void* pv);
//itcm
extern char _text_end[];
extern uint32_t __free_iram[];

#define SMF_DTCM_POOL_SIZE 60*1024

EXTERNC void smf_project_memory() {
    //shared memory
    // smf_register_ics((void*)SMF_AON_RAM_BASE, SMF_AON_RAM_SIZE, false);
    uint32_t ics_buffer;
    uint32_t ics_buflen;
    int ret = rptun_get_ics_buffer(RPTUN_CHAN_M55C0_2_DSPC0, &ics_buffer, &ics_buflen);
    if (ret || !ics_buffer || !ics_buflen) {
        dbgErrPXL("[HIFI5S]get ics buffer failed");
		dbgErrPXL("ret/buf/len %u/%u/%u", ret, ics_buffer, ics_buflen);
        return;
    }

    dbgInfoPXL("ics:0x%08x,0x%08x", ics_buffer, ics_buflen);
    smf_register_ics((void*)ics_buffer, ics_buflen, false);

    auto psram_pool = (char*)((((uint32_t)_dpsram_heap_start + 7) >> 3 << 3) + 1024);
    auto psram_size = (uint32_t)(_dpsram_heap_end - psram_pool);
    dbgInfoPXL("psram:0x%08x,0x%08x/0x%08x", psram_pool, psram_size, HIFI4_PSRAM_SIZE);
    smf_register_pool("psram", psram_pool, psram_size, 1024 * 16);

    auto dtcm_pool = malloc(SMF_DTCM_POOL_SIZE);
    if(!dtcm_pool){
        dbgErrPXL("malloc dtcm pool failed");
        return;
    }
    dbgInfoPXL("dram:0x%08x,0x%08x/0x%08x", dtcm_pool, SMF_DTCM_POOL_SIZE, HIFI4_DTCM_SIZE);
    smf_register_pool("dtcm", dtcm_pool, SMF_DTCM_POOL_SIZE, 1024 * 4);

    auto itcm_pool = (void*)(((uint32_t)_text_end + 7) >> 3 << 3);
    auto itcm_size = (uint32_t)__free_iram;
    dbgInfoPXL("iram:0x%08x,0x%08x/0x%08x", itcm_pool, itcm_size, HIFI4_ITCM_SIZE);
    smf_register_pool_text("itcm", itcm_pool, itcm_size);

    smf_register_pool_callback("sys", &pvPortMalloc, &vPortFree, &xPortGetFreeHeapSize, &xPortGetTotalHeapSize);

    smf_designate_globle_pool("psram");
}
//
EXTERNC void smf_project_register() {
    //
    SMF_REGISTER(background);
    SMF_REGISTER(pipeline);
    SMF_REGISTER(muxer);
    SMF_REGISTER(demuxer);
    SMF_REGISTER(decoder);
    SMF_REGISTER(encoder);
    SMF_REGISTER(dumy_source);
    SMF_REGISTER(dumy_sink);
    //SMF_REGISTER(io_source);
    //SMF_REGISTER(io_sink);
    //SMF_REGISTER(func_source);
    //SMF_REGISTER(func_sink);
    SMF_REGISTER(notify_bg);

    //stream server
    SMF_REGISTER(audio_player);
    SMF_REGISTER(audio_record);
    SMF_REGISTER(stream_server);

    //msg service
    SMF_REGISTER(msg_service);
    //SMF_REGISTER(msg_service_audio_player);
    //SMF_REGISTER(msg_service_audio_record);
    //SMF_REGISTER(msg_client_audio_player_remote);
    //SMF_REGISTER(msg_client_audio_record_remote);
    //SMF_REGISTER(msg_client_avrecord_remote);

    //SMF_REGISTER(msg_client_status);
    //SMF_REGISTER(msg_client_sync);
    //SMF_REGISTER(msg_client_device_local);
    SMF_REGISTER(msg_client_core);
    // SMF_REGISTER(msg_client_fs);
    // SMF_REGISTER(msg_client_pool_remote);
    SMF_REGISTER(msg_client_pool);
    // SMF_REGISTER(msg_client_pool1);
    SMF_REGISTER(msg_client_hook_client);

    //SMF_REGISTER(msg_client_hook_remote);
    //
    SMF_REGISTER(fifo_filter);
    SMF_REGISTER(fifo_buffer);
    SMF_REGISTER(pcm_1xN);
    SMF_REGISTER(frame_buffer);
    // SMF_REGISTER(button_source);

    //communication element
    SMF_REGISTER(shm_source);
    SMF_REGISTER(shm_sink);

    SMF_REGISTER(libs_sink);
    SMF_REGISTER(libs_filter);

    //dma device
    SMF_REGISTER(dma_source);
    SMF_REGISTER(dma_sink);
    SMF_REGISTER(microphone);
    SMF_REGISTER(i2s_source);
    SMF_REGISTER(i2s_sink);

    //audio element
    SMF_REGISTER(mix);
    SMF_REGISTER(pcm_format);
    SMF_REGISTER(pcm_adjust);
    SMF_REGISTER(plc);
    SMF_REGISTER(pcm_preset);
    SMF_REGISTER(volume_ctrl);

    //audio enc/dec
    SMF_CODEC_REGISTER(enc, pcm);
    SMF_CODEC_REGISTER(dec, pcm);

    SMF_CODEC_REGISTER(enc, msbc);
    SMF_CODEC_REGISTER(dec, msbc);
    SMF_CODEC_REGISTER(enc, cvsd);
    SMF_CODEC_REGISTER(dec, cvsd);
    SMF_CODEC_REGISTER(enc, sbc);
    SMF_CODEC_REGISTER(dec, sbc);
    SMF_CODEC_REGISTER(dec, mp3);
    SMF_CODEC_REGISTER(enc, aac);
    SMF_CODEC_REGISTER(dec, aac);

    // SMF_CODEC_REGISTER(enc, opus);
    // SMF_CODEC_REGISTER(dec, opus);

    SMF_CODEC_REGISTER(dec, h264);

    //audio resample
    SMF_REGISTER(resample);
    SMF_REGISTER(resample_coef);
    SMF_RESAMPLE_COEF_REGISTER(8000, 48000);
    SMF_RESAMPLE_COEF_REGISTER(16000, 48000);
    SMF_RESAMPLE_COEF_REGISTER(24000, 48000);
    SMF_RESAMPLE_COEF_REGISTER(32000, 48000);
    SMF_RESAMPLE_COEF_REGISTER(22050, 48000);
    SMF_RESAMPLE_COEF_REGISTER(44100, 48000);
    SMF_RESAMPLE_COEF_REGISTER(48000, 16000);

    //bt element
    SMF_REGISTER(sco_source);
    SMF_REGISTER(sco_sink);
    // SMF_REGISTER(intsco_source);
    // SMF_REGISTER(intsco_sink);

    SMF_REGISTER(tswait_filter);
	SMF_REGISTER(pixfmt);
	SMF_REGISTER(i420p16s_i420);
    SMF_REGISTER(display);
    // SMF_REGISTER(i420p16_u420p8);
    // SMF_REGISTER(i420p16_u420p16);

    //algo
    //SMF_REGISTER(sco_algo);
    SMF_REGISTER(echo_source);
    SMF_REGISTER(a2dp_source);
    SMF_REGISTER(a2dp_sink);
    //SMF_REGISTER(audio_process);
    //SMF_REGISTER(speech_algo);
    // SMF_REGISTER(call_record);
#ifdef SMF_HAS_DYNLIB
    SMF_REGISTER(dynamic_loader);
#endif
    // SMF_LIB_REGISTER(pa);
#ifdef SMF_AUDIO_PROCESS
    SMF_REGISTER(audio_process);
#endif
}

EXTERNC bool smf_msg_service_open() {
    smf_init();
    smf_set_cpu(SMF_CPU_DSP,"hifi5s");
    smf_project_memory();
    smf_project_register();
    // smf_rpmsg_register_boot_cb(
    // 	[](const char *name)->bool {
    // 		dbgTestPXL("connecting %s", name);
    // 		bes_rpchan_init(RPTUN_CHAN_M55C1_2_DSPC0);
    // 		//smf_log_redirect_to_background(_log_service_buff, sizeof(_log_service_buff));
    // 		dbgTestPXL("connected");
    // 		return true;
    // 	},
    // 	[](const char *name)->bool {
    // 		dbgTestPXL("disconnecting %s", name);
    // 		dbgTestPXL("disconnected");
    // 		return true;
    // 	}
    // );
    // returnIfErrC(false, !smf_config_from_ics());
    // smf_ics_ready();

    // smf::thread thread;
    // thread.Start("smfinit", [](void* priv) {
        // returnIfErrC0(!smf_ics_sync_ready(1, 60000, 10));
        // returnIfErrC0(!smf_ics_sync_ready(2, 60000, 10));
    returnIfErrC(false,!smf_ics_sync_ready(SMF_CPU_AP, 1000, 5));
    smf_config_from_ics();
    smf_ics_ready();
        // }, 0, 1024 * 16);
    return true;
}

EXTERNC bool smf_msg_service_close() {
    smf_uninit();
    return true;
}

EXTERNC bool smf_project_init() {
    smf_msg_service_open();
    return true;
}

EXTERNC bool smf_project_entry(int argc, char* argv[]) {
    smf_cmd_entry(argc, argv);
    return true;
}
