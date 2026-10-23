#pragma once
#include <smf_common.h>
#include <smf_frame.h>
#include <smf_error.h>

/**
 * @brief media frame callback function type.
*/
typedef bool (*smf_media_frame_cb)(void* priv, smf_frame_t* frm);

/**
 * @brief media message structure.
*/
typedef struct smf_media_message_t {
	void* priv;					/**< [in]private data for callback */
	void* handle;				/**< [in]media handle */
	uint8_t msgid;				/**< [in]message ID, refer to smf_media_message_operator_e */
	uint8_t stream;				/**< [in]stream ID */
	uint16_t data_size;			/**< [in]size of data */
	void* data;					/**< [in]message data */
}smf_media_message_t;

/**
 * @brief media message callback function type.
*/
typedef bool (*smf_media_message_cb)(smf_media_message_t* msg);

/**
 * @brief media initialization parameters.
*/
typedef struct smf_media_init_t {
	smf_media_message_cb cbmsg;	/**< [in]message callback function */
	void* cbmsgpriv;				/**< [in]private data for message callback */
}smf_media_init_t;

/**
 * @brief initialize media module.
 * @param param [in]initialization parameters.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_init(struct smf_media_init_t* param);

/**
 * @brief deinitialize media module.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_deinit();

/**
 * @brief get the media manager error
 * @return the last error.
 */
EXTERNC smf_error_t* smf_media_error();

//handle operations

/**
 * @brief media open parameters.
*/
typedef struct smf_media_open_t {
	smf_media_message_cb cbmsg;	/**< [in]message callback function */
	void* cbmsgpriv;				/**< [in]private data for message callback */
	uint32_t timeout;				/**< [in]operation timeout in milliseconds */
	const char* scene;				/**< [in]usage scene: record, wakeup, detect, music, notify, prompt, sco, ring, alarm */
	const char* app;				/**< [in]application identifier, e.g., "audio_player_url" */
	bool autofree;					/**< [in]automatically free resources when not needed */
	bool free_cbmsgpriv;
}smf_media_open_t;

struct smf_media_prepare_t;

/**
 * @brief create a media instance.
 * @param param [in]media open parameters.
 * @return media handle if successful, NULL otherwise.
*/
EXTERNC void* smf_media_create(struct smf_media_open_t* param);

/**
 * @brief destroy a media instance.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_destroy(void* handle);

/**
 * @brief prepare media instance with parameters.
 * @param handle [in]media handle.
 * @param param [in]preparation parameters.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_prepare(void* handle, struct smf_media_prepare_t* param);

/**
 * @brief prepare media instance with string parameters.
 * @param handle [in]media handle.
 * @param param [in]preparation string parameters..
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_prepare_string(void* handle, const char* param);

/**
 * @brief append preparation parameters to media instance.
 * @param handle [in]media handle.
 * @param param [in]additional parameters in string format.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_prepare_append(void* handle, const char* param);

/**
 * @brief start media playback/recording.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_start(void* handle);

/**
 * @brief stop media playback/recording.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_stop(void* handle);

/**
 * @brief pause media playback.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_pause(void* handle);

/**
 * @brief resume media playback from pause state.
 * @warning only valid after pause operation.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_resume(void* handle);

/**
 * @brief push frame to media instance.
 * @warning typically used for push-mode recording.
 * @param handle [in]media handle.
 * @param frame [in]media frame data.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_push(void* handle, smf_frame_t* frame);

/**
 * @brief pull frame from media instance.
 * @warning typically used for pull-mode playback.
 * @param handle [in]media handle.
 * @param frame [out]media frame data.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_pull(void* handle, smf_frame_t* frame);

/**
 * @brief set parameter to media instance.
 * @param handle [in]media handle.
 * @param setid [in]parameter set ID.
 * @param params [in]parameter data address.
 * @param size [in]parameter data size.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_set_param(void* handle, uint32_t setid, const void* params, uint32_t size);

/**
 * @brief get parameter from media instance.
 * @param handle [in]media handle.
 * @param getid [in]parameter get ID.
 * @param params [out]buffer for parameter data.
 * @param size [in,out]input: buffer size, output: actual data size.
 * @param timeout [in]timeout in milliseconds.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_param(void* handle, uint32_t getid, void* params, uint32_t* size, uint32_t timeout);

/**
 * @brief get media instance status.
 * @param handle [in]media handle.
 * @param status [out]status value.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_status(void* handle, uint32_t* status);

/**
 * @brief get media app type.
 * @param handle [in]media handle.
 * @return the app type.
*/
EXTERNC const char* smf_media_get_type(void* handle);

/**
 * @brief get the app error
 * @param handle [in]media handle.
 * @return the last error.
 */
EXTERNC smf_error_t* smf_media_get_error(void* handle);

//audio operations

/**
 * @brief set audio volume.
 * @param handle [in]media handle.
 * @param volume [in]volume value.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_set_volume(void* handle, uint32_t volume);

/**
 * @brief reduce audio volume (e.g., for ducking).
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_reduce_volume(void* handle);

/**
 * @brief recover audio volume to previous level.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_recover_volume(void* handle);

/**
 * @brief set audio mute state.
 * @param handle [in]media handle.
 * @param mute [in]true to mute, false to unmute.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_set_mute(void* handle, bool mute);

/**
 * @brief force mute audio (e.g., for priority audio).
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_force_mute(void* handle);

/**
 * @brief recover from forced mute state.
 * @param handle [in]media handle.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_recover_mute(void* handle);

/**
 * @brief get current audio volume.
 * @param handle [in]media handle.
 * @param volume [out]current volume value.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_volume(void* handle, uint32_t* volume);

/**
 * @brief get current audio mute state.
 * @param handle [in]media handle.
 * @param mute [out]current mute state.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_mute(void* handle, bool* mute);

/**
 * @brief set sco (Synchronous Connection Oriented) audio volume.
 * @param handle [in]media handle.
 * @param volume [in]volume value for sco audio.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_set_scoup_volume(void* handle, uint32_t volume);

/**
 * @brief set sco (Synchronous Connection Oriented) audio mute state.
 * @param handle [in]media handle.
 * @param mute [in]true to mute, false to unmute.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_set_scoup_mute(void* handle, bool mute);

//player operations

/**
 * @brief seek to specific time position in media playback.
 * @warning only valid for file playback.
 * @param handle [in]media handle.
 * @param timepoint [in]target time position in milliseconds.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_seek(void* handle, uint32_t timepoint);

/**
 * @brief get current playback position.
 * @param handle [in]media handle.
 * @param timepoint [out]current time position in milliseconds.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_position(void* handle, uint32_t* timepoint);

/**
 * @brief get media playback progress information.
 * @param handle [in]media handle.
 * @param progress [out]pointer to progress structure.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_progress(void* handle, smf_progress_t** progress);

/**
 * @brief get media metadata information.
 * @param handle [in]media handle.
 * @param meta [out]pointer to metadata structure.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_get_meta_info(void* handle, smf_meta_info_t** meta);

/**
 * @brief parse metadata information from file.
 * @param url [in]file path or URL.
 * @param pmeta [out]pointer to metadata structure.
 * @param tempbuff [in]temporary buffer for metadata parsing.
 * @param tempsize [in]size of temporary buffer, recommended: 1024.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_meta_parse(const char* url, smf_meta_info_t* pmeta, void* tempbuff, uint32_t tempsize);

//device operations

/**
 * @brief media device type enumeration.
*/
typedef enum smf_media_device_type_e {
	SMF_MEDIA_DEVICE_AudioOutput = 1,	/**< Audio output device */
	SMF_MEDIA_DEVICE_AudioInput,		/**< Audio input device */
	SMF_MEDIA_DEVICE_VideoOutput,		/**< Video output device */
	SMF_MEDIA_DEVICE_VideoInput,		/**< Video input device */
}smf_media_device_type_e;

/**
 * @brief switch media device.
 * @param type [in]device type.
 * @param device [in]device identifier.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_device_switch(smf_media_device_type_e type, const char* device);

/**
 * @brief start media device.
 * @param type [in]device type.
 * @param device [in]device identifier.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_device_start(smf_media_device_type_e type, const char* device);

/**
 * @brief stop media device.
 * @param type [in]device type.
 * @param device [in]device identifier.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_device_stop(smf_media_device_type_e type, const char* device);

/**
 * @brief set device volume.
 * @param type [in]device type.
 * @param volume [in]volume value.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_device_set_volume(smf_media_device_type_e type, uint32_t volume);

/**
 * @brief set device mute state.
 * @param type [in]device type.
 * @param mute [in]true to mute, false to unmute.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_device_set_mute(smf_media_device_type_e type, bool mute);

/**
 * @brief set device volume.
 * @param scene [in]scene type: record, wakeup, detect, music, notify, prompt, sco, ring, alarm
 * @param volume [in]volume value.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_scene_set_volume(const char* scene, uint32_t volume);

/**
 * @brief set device mute state.
 * @param scene [in]scene type: record, wakeup, detect, music, notify, prompt, sco, ring, alarm
 * @param mute [in]true to mute, false to unmute.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_scene_set_mute(const char* scene, bool mute);

//hook operations

/**
 * @brief register hook callback for media data interception.
 * @param callback [in]hook callback function.
 * @param priv [in]private data for callback.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_hook2_register(smf_hook_cb callback, void* priv);

/**
 * @brief start hook flow.
 * @param id [in]hook ID, refer to configuration.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_hook2_start(uint32_t id);

/**
 * @brief stop hook flow.
 * @param id [in]hook ID.
 * @return true if successful, false otherwise.
*/
EXTERNC bool smf_media_hook2_stop(uint32_t id);

/**
 * @brief media message operator enumeration.
*/
enum smf_media_message_operator_e{
	SMF_MEDIA_MESSAGE_eos = 0x01,		/**< End of stream */
	SMF_MEDIA_MESSAGE_error,			/**< Error occurred */
	SMF_MEDIA_MESSAGE_done,				/**< Operation completed */
	SMF_MEDIA_MESSAGE_status,			/**< Status update */
	SMF_MEDIA_MESSAGE_progress,			/**< Progress update */
	SMF_MEDIA_MESSAGE_meta,				/**< Metadata update */
	SMF_MEDIA_MESSAGE_media,			/**< Media data */
	SMF_MEDIA_MESSAGE_start_sucess,		/**< Start success */
	SMF_MEDIA_MESSAGE_start_fail,		/**< Start failure */
	SMF_MEDIA_MESSAGE_vad = 0x10,		/**< Voice activity detection */
	SMF_MEDIA_MESSAGE_wakeup,			/**< Wakeup detection */
	SMF_MEDIA_MESSAGE_aivioce,			/**< AI voice event */
	SMF_MEDIA_MESSAGE_kws,				/**< Keyword spotting */
	SMF_MEDIA_MESSAGE_stop_sucess,		/**< Stop success */
	SMF_MEDIA_MESSAGE_stop_fail,		/**< Stop failure */
	SMF_MEDIA_MESSAGE_voip,				/**< VoIP event */

	SMF_MEDIA_MESSAGE_EXECUTE_VolumeDown = 0x41,	/**< Execute volume down */
	SMF_MEDIA_MESSAGE_EXECUTE_Mute,					/**< Execute mute */
	SMF_MEDIA_MESSAGE_EXECUTE_Pause,				/**< Execute pause */
	SMF_MEDIA_MESSAGE_EXECUTE_Stop,					/**< Execute stop */

	SMF_MEDIA_MESSAGE_RECOVER_VolumeDown = 0x51,	/**< Recover from volume down */
	SMF_MEDIA_MESSAGE_RECOVER_Mute,					/**< Recover from mute */
	SMF_MEDIA_MESSAGE_RECOVER_Pause,				/**< Recover from pause */
	SMF_MEDIA_MESSAGE_RECOVER_Stop,					/**< Recover from stop */

	SMF_MEDIA_MESSAGE_A2DP_Ready = 0x60,	/**< A2DP ready */
	SMF_MEDIA_MESSAGE_A2DP_Unready,			/**< A2DP not ready */
};