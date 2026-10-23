#pragma once
#include <smf_common.h>
#include <smf_frame.h>
#include <smf_error.h>

typedef bool (*smf_bt_frame_cb)(smf_frame_t* frm);

typedef struct smf_bt_message_t {
	void* priv;
	void* handle;
	uint8_t msgid;//smf_bt_message_e
	uint8_t stream;
	uint16_t data_size;
	void* data;
}smf_bt_message_t;
typedef bool (*smf_bt_message_cb)(smf_bt_message_t* msg);

#define SMF_BT_TYPE_A2DPSource "audio_a2dp_source"
#define SMF_BT_TYPE_A2DPSink "audio_a2dp_sink"
#define SMF_BT_TYPE_BTSCO "audio_sco_bt"
#define SMF_BT_TYPE_AGSCO "audio_sco_ag"
typedef struct smf_bt_create_t {
	smf_bt_message_cb cbmsg;
	void* cbmsgpriv;
	const char* type;//SMF_BT_TYPE_xxx
}smf_bt_create_t;

typedef struct smf_bt_prepare_t {
	uint32_t volume;
	uint32_t mute;
	union {
		union {
			struct {
				const char* fmt;
				const char* enc;
			}sink;
			struct {
				const char* media;
			}source;
		}a2dp;
		union {
			struct {
				const char* src;
				const char* dec;
				const char* fmt;
				const char* enc;
			}bt;
			struct {
				const char* codec_dn;
				const char* codec_up;
			}ag;
		}sco;
	};
}smf_bt_prepare_t;

//handle
EXTERNC void* smf_bt_create(struct smf_bt_create_t* param);
EXTERNC bool smf_bt_destroy(void* handle);
EXTERNC bool smf_bt_prepare(void* handle, struct smf_bt_prepare_t* param);
EXTERNC bool smf_bt_prepare_string(void* handle, const char* param);
EXTERNC bool smf_bt_prepare_append(void* handle, const char* param);
EXTERNC bool smf_bt_start(void* handle);
EXTERNC bool smf_bt_stop(void* handle);
EXTERNC bool smf_bt_start_a2dpup(void* handle);
EXTERNC bool smf_bt_stop_a2dpup(void* handle);
EXTERNC bool smf_bt_pause(void* handle);
EXTERNC bool smf_bt_resume(void* handle);
EXTERNC bool smf_bt_pull(void* handle, smf_frame_t* frame);
EXTERNC bool smf_bt_push(void* handle, smf_frame_t* frame);
EXTERNC const char* smf_bt_get_type(void* handle);
EXTERNC smf_error_t* smf_bt_get_error(void* handle);

//volume
EXTERNC bool smf_bt_set_volume(void* handle, uint32_t volume);
EXTERNC bool smf_bt_reduce_volume(void* handle);
EXTERNC bool smf_bt_recover_volume(void* handle);
EXTERNC bool smf_bt_set_mute(void* handle, bool mute);
EXTERNC bool smf_bt_force_mute(void* handle);
EXTERNC bool smf_bt_recover_mute(void* handle);
EXTERNC bool smf_bt_get_volume(void* handle, uint32_t* volume);
EXTERNC bool smf_bt_get_mute(void* handle, bool* mute);

//sco
EXTERNC bool smf_bt_set_scoup_volume(void* handle, uint32_t volume);
EXTERNC bool smf_bt_set_scoup_mute(void* handle, bool mute);

//msg
enum smf_bt_message_e{
	SMF_BT_MESSAGE_VolumeDown = 0x41,
	SMF_BT_MESSAGE_Mute = 0x42,
	SMF_BT_MESSAGE_Pause = 0x43,
	SMF_BT_MESSAGE_Stop = 0x44,

	SMF_BT_MESSAGE_VolumeRecover = 0x51,
	SMF_BT_MESSAGE_MuteRecover = 0x52,
	SMF_BT_MESSAGE_Resume = 0x53,
	SMF_BT_MESSAGE_Start = 0x54,
};
