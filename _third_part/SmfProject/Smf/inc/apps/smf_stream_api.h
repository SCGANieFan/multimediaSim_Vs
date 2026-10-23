#pragma once
#include "smf_common.h"
#include "smf_media.h"
#include "smf_frame.h"
#define SMF_STREAM_MAX 128
#define SMF_STREAM_SETID_MAX 32
#define SMF_STREAM_NOTIFY_MAX 32
#define SMF_STREAM_HOOK_MAX 64

/**
 * @brief the message ID for notify.
*/
typedef enum{
	SMF_STREAM_MESSAGE_begin = 0,
	SMF_STREAM_MESSAGE_eos,		//0x01
	SMF_STREAM_MESSAGE_error,	//0x02
	SMF_STREAM_MESSAGE_done,	//0x03
	SMF_STREAM_MESSAGE_status,	//0x04
	SMF_STREAM_MESSAGE_progress,//0x05
	SMF_STREAM_MESSAGE_meta,	//0x06
	SMF_STREAM_MESSAGE_media,	//0x07
	SMF_STREAM_MESSAGE_start_sucess,	//0x08
	SMF_STREAM_MESSAGE_start_fail,	//0x09
	SMF_STREAM_MESSAGE_vad = 0x10,//0x10
	SMF_STREAM_MESSAGE_wakeup,	//0x11
	SMF_STREAM_MESSAGE_aivioce,	//0x12
	SMF_STREAM_MESSAGE_kws,//0x13
	SMF_STREAM_MESSAGE_stop_sucess,//0x14
	SMF_STREAM_MESSAGE_stop_fail,//0x15
	SMF_STREAM_MESSAGE_voip,//0x16
	SMF_STREAM_MESSAGE_end,		//
}smf_stream_message_e;

/**
 * @brief the status of streaming.
*/
typedef enum {
	SMF_STREAM_STATUS_null = 0,
	SMF_STREAM_STATUS_ready,//1
	SMF_STREAM_STATUS_play,//2
	SMF_STREAM_STATUS_pause,//3
}smf_stream_status_e;
typedef enum {
	SMF_STREAM_MESSAGE_CALLBACK_0 = 0,
	SMF_STREAM_MESSAGE_CALLBACK_1,//1
	SMF_STREAM_MESSAGE_CALLBACK_2,//2
	SMF_STREAM_MESSAGE_CALLBACK_3,//3
	SMF_STREAM_MESSAGE_CALLBACK_end,
}smf_stream_message_callback_e;

/**
 * @brief the message callbacke paramer.
*/
typedef struct smf_stream_message_t {
	void* priv;
	uint8_t msgid;
	uint8_t stream;
	uint8_t seqno;
	uint8_t data_size;
	char data[0];
}smf_stream_message_t;
typedef void(*smf_stream_message_cb)(smf_stream_message_t* msg);

/**
 * @brief register callback to receive msg.
 * @param id [in]smf_stream_message_callback_e
 * @param cb [in]smf_stream_message_cb
 * @param priv [in]callback priv data
 * @return true/false
*/
EXTERNC bool smf_stream_register(smf_stream_message_callback_e id, smf_stream_message_cb cb, void* priv);

/**
 * @brief register callback to receive msg.
			the message callback function is delegated to the bg thread for execution,
			without occupying the main message thread.
 * @param id [in]smf_stream_message_callback_e
 * @param cb [in]smf_stream_message_cb
 * @param priv [in]callback priv data
 * @return true/false
*/
EXTERNC bool smf_stream_register_async(smf_stream_message_callback_e id, smf_stream_message_cb cb, void* priv);

/**
 * @brief start one stream.
 * @param stream [in]the stream ID.
 * @return true/false
 * @see smf_demo_simple_player()
*/
EXTERNC bool smf_stream_start(uint32_t stream);

/**
 * @brief start one stream.
 *	and waiting for the initiation process to complete.
 * @param stream [in]the stream ID.
 * @param timeout [in]timeout (unit: milliseconds).
 * @return true/false
*/
EXTERNC bool smf_stream_start_wait(uint32_t stream, uint32_t timeout);

/**
 * @brief start one stream.
 * @param stream [in]the stream ID.
 * @param param [in]the param address.
 * @param size [in]the param size.
 * @return true/false
*/
EXTERNC bool smf_stream_start_params(uint32_t stream, const void* param, uint32_t size);

/**
 * @brief start one stream.
 *	and waiting for the initiation process to complete.
 * @param stream [in]the stream ID.
 * @param param [in]the param address.
 * @param size [in]the param size.
 * @param timeout [in]timeout (unit: milliseconds).
 * @return true/false
 * @see smf_demo_simple_player()
*/
EXTERNC bool smf_stream_start_params_wait(uint32_t stream, const void* param, uint32_t size, uint32_t timeout);

/**
 * @brief start one stream.
 * @param stream [in]the stream ID.
 * @param param [in]the script param.
 * @return true/false
 * @see smf_demo_simple_player()
*/
EXTERNC bool smf_stream_start_string(uint32_t stream, const char* param);

/**
 * @brief start one stream.
 *	and waiting for the initiation process to complete.
 * @param stream [in]the stream ID.
 * @param param [in]the script param.
 * @param timeout [in]timeout (unit: milliseconds).
 * @return true/false
*/
EXTERNC bool smf_stream_start_string_wait(uint32_t stream, const char* param, uint32_t timeout);

/**
 * @brief stop one stream.
 * @param stream [in]the stream ID.
 * @return true/false
*/
EXTERNC bool smf_stream_stop(uint32_t stream);

/**
 * @brief pause one stream.
 * @param stream [in]the stream ID.
 * @return true/false
*/
EXTERNC bool smf_stream_pause(uint32_t stream);

/**
 * @brief resume one stream.
 * @warning	only be used after the pause state
 * @param stream [in]the stream ID.
 * @return true/false
*/
EXTERNC bool smf_stream_resume(uint32_t stream);

/**
 * @brief seek streaming to giving position.
 * @warning	only be used in file-play
 * @param stream [in]the stream ID.
 * @param timepoint [in]the timepoint position (unit: milliseconds).
 * @return true/false
*/
EXTERNC bool smf_stream_seek(uint32_t stream, uint32_t timepoint);

/**
 * @brief Set the param to streaming.
 * @param stream [in]the stream ID.
 * @param keys [in]the Set ID.
 *	Refer to the "sets" table in smf.json.
 * @param data [in]the param address
 * @param size [in]the param size
 * @return true/false
*/
EXTERNC bool smf_stream_set(uint32_t stream, uint32_t keys, void* data, uint32_t size);

/**
 * @brief Get the param from streaming.
 * @param stream [in]the stream ID.
 * @param keys [in]the Get ID.
 *	Refer to the "sets" table in smf.json.
 * @param data [out]the param address
 * @param max [in]the param buffer max size
 * @return true/false
*/
EXTERNC bool smf_stream_get(uint32_t stream, uint32_t keys, void* data, uint32_t max, uint32_t timeout);

/**
 * @brief Get the param from streaming.
 *	Asynchronous interface, with results returned by callback functions.
 * @param stream [in]the stream ID.
 * @param keys [in]the Get ID.
 *	Refer to the "sets" table in smf.json.
 * @param cb [out]the callback for output.
 * @param priv [in]the private data of callback.
 * @return true/false
*/
EXTERNC bool smf_stream_get_async(uint32_t stream, uint32_t keys, smf_cb_data cb, void* priv);

/**
 * @brief Push frame to streaming.
 * @warning	Only used for push-player.
 * @param stream [in]the stream ID.
 * @param frm [in]the data frame.
 * @return true/false.
*/
EXTERNC bool smf_stream_push(uint32_t stream, smf_frame_t* frm);

/**
 * @brief Pull frame from streaming.
 * @warning	Only used for pull-recorder.
 * @param stream [in]the stream ID.
 * @param frm [out]the data frame.
 * @return true/false
*/
EXTERNC bool smf_stream_pull(uint32_t stream, smf_frame_t* frm);

/**
 * @brief register a callback to receive hook data.
 * @param cb [out]hook callback.
 * @param priv [in]the private data of callback.
 * @return true/false.
*/
EXTERNC bool smf_stream_hook_register(smf_hook_cb cb, void* priv);

/**
 * @brief start a hook flow.
 * @param pointer [in]the hook ID.
 *  Refer to the "hook" table in smf.json.
 * @return true/false.
*/
EXTERNC bool smf_stream_hook_start(uint32_t pointer);

/**
 * @brief stop a hook flow.
 * @param pointer [in]the hook ID.
 * @return true/false.
*/
EXTERNC bool smf_stream_hook_stop(uint32_t pointer);

/**
 * @brief check a streaming is used.
 * @param stream [in]the stream ID.
 * @return true/false
*/
EXTERNC bool smf_stream_check(uint32_t stream);

/**
 * @brief get the name of streaming.
 * @param stream [in]the stream ID.
 * @return the name string.
*/
EXTERNC const char* smf_stream_get_name(uint32_t stream);

/**
 * @brief get the name(uint64_t) of streaming.
 * @param stream [in]the stream ID.
 * @return the name uint64_t.
*/
EXTERNC uint64_t smf_stream_get_name64(uint32_t stream);

/**
 * @brief get a free streaming ID.
 * @param begin [in]the begin of stream ID.
 * @param end [in]the end of stream ID.
 * @return true/false.
*/
EXTERNC uint32_t smf_stream_get_free(uint32_t begin, uint32_t end);

/**
 * @brief get stream seqNO.
 * @param stream [in]the stream ID.
 * @return the seqNO.
*/
EXTERNC uint32_t smf_stream_get_seqno(uint32_t stream);

/**
 * @brief Set a 32-bits interger parameter to streaming.
 * @param stream [in]the stream ID.
 * @param keys [in]the set ID.
 * @param value [in]the param value.
 * @return true/false.
*/
EXTERNC bool smf_stream_set32(uint32_t stream, uint32_t keys, uint32_t value);

/**
 * @brief Set a 32-bits interger parameter to streaming.
 * @param stream [in]the stream ID.
 * @param keys [in]the set ID.
 * @param value [in]the param value.
 * @param timeout [in]timeout (unit: milliseconds).
 * @return true/false.
*/
EXTERNC bool smf_stream_get32(uint32_t stream, uint32_t keys, uint32_t* value, uint32_t timeout);

/**
 * @brief Set volume to audio streaming.
 * @param stream [in]the stream ID.
 * @param value [in]the value of volume.
 *	The value range is [0~32768],
 *  Represents the linear amplification coefficient [0.0~1.0].
 * @return
*/
EXTERNC bool smf_stream_set_volume(uint32_t stream, uint32_t value);
EXTERNC bool smf_stream_set_volume_reduce(uint32_t stream, bool reduce);

/**
 * @brief Set/Release mute state to audio streaming.
 * @param stream [in]the stream ID.
 * @param mute [in]mute state.
 * @return true/false.
*/
EXTERNC bool smf_stream_set_mute(uint32_t stream, bool mute);
EXTERNC bool smf_stream_set_mute_force(uint32_t stream, bool force);

/**
 * @brief Temporarily store a set of data,
 *	and automatically release it upon timeout.
 *  commonly used to set the initialization parameters of algorithms.
 * @param name [in]the unique name.
 * @param data [in]the data address.
 * @param size [in]the data size.
 * @param timeout [in]timeout (unit: milliseconds).
 * @return true/false
*/
EXTERNC bool smf_stream_store_param(const char* name, void* data, int size, uint32_t timeout);

/**
 * @brief parse meta info from file
 * @param url [in]the file name.
 * @param pmeta [out]the meta info pointer.
 * @param tempbuff [in]the temp buff for meta info.
 * @param tempsize [in]the size of temp buff, the recommended value: 1024.
 * @return true/false
*/
EXTERNC bool smf_stream_meta_parse(const char* url, smf_meta_info_t* pmeta, void* tempbuff, uint32_t tempsize);

/**
 * @brief hold a free streaming.
 * @param begin [in]the begin of stream ID.
 * @param end [in]the end of stream ID.
 * @return stream ID.
 * @note call @smf_stream_release to release stream.
 * @delete
*/
//EXTERNC uint32_t smf_stream_select_and_hold(uint32_t begin, uint32_t end);

/**
 * @brief hold a free streaming.
 * @param stream [in]the stream ID.
 * @return true/false.
 * @note call @smf_stream_release to release stream.
 * @delete
*/
//EXTERNC bool smf_stream_hold(uint32_t stream);

/**
 * @brief release streaming.
 * @param stream [in]the stream ID.
 * @return true/false.
 * @delete
*/
//EXTERNC bool smf_stream_release(uint32_t stream);

/**
 * @brief the demo for the simple player.
 * @code
#include "smf_stream_api.h"
#include "smf_stream_api_default.h"
bool smf_demo_simple_player() {
	//get a free streaming for player.
	uint32_t stream = smf_stream_get_free(SMF_STREAM_PLAY1, SMF_STREAM_PLAY3);

	//check stream valid.
	if (!stream)return false;

	//create the params.
	char params[128];
	const char* url = "/emmc/music.m4a";
	smf_meta_info_t meta;
	sprintf(params, "dem=[url=[%s],meta=[%p]", url, &meta);

	//start the player with params.
	bool rst = smf_stream_start_string(stream, params);
	if (!rst)return false;

	//print meta info
	if (meta.url)printf("url:%s", meta.url);
	if (meta.album)printf("album:%s", meta.album);
	if (meta.artist)printf("artist:%s", meta.artist);
	if (meta.title)printf("title:%s", meta.title);
	smf_sleep_for(5);

	//set volume for player
	rst = smf_stream_set_volume(stream, 16384);
	smf_sleep_for(5);

	//set mute for player.
	rst = smf_stream_set_mute(stream, true);
	smf_sleep_for(5);

	//release mute for player.
	rst = smf_stream_set_mute(stream, false);
	smf_sleep_for(5);

	//get the current position for player.
	printf("frameindex:%u/%u", meta.frameindex->current, meta.frameindex->end);
	printf("timepoint:%u/%u", meta.timepoint->current, meta.timepoint->end);
	smf_sleep_for(5);

	printf("frameindex:%u/%u", meta.frameindex->current, meta.frameindex->end);
	printf("timepoint:%u/%u", meta.timepoint->current, meta.timepoint->end);
	smf_sleep_for(5);

	//seek to the giving position
	rst = smf_stream_seek(stream, 100);
	printf("frameindex:%u/%u", meta.frameindex->current, meta.frameindex->end);
	printf("timepoint:%u/%u", meta.timepoint->current, meta.timepoint->end);
	smf_sleep_for(5);

	//stop player
	rst = smf_stream_stop(stream);
	return rst;
}
 * @endcode
 */
EXTERNC bool smf_demo_simple_player(void);
