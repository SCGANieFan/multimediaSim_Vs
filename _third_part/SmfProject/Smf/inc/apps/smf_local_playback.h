#pragma once
#ifndef __SMF_LOCAL_PLAYBACK_H__
#define __SMF_LOCAL_PLAYBACK_H__

#include "com/smf_common.h"
#include "com/smf_media.h"
#include "com/smf_error.h"

enum smf_local_playback_mode_e {
	eSmfLocalPlayerModeNull = 0,
	eSmfLocalPlayerModeSingle,
	eSmfLocalPlayerModeSingleLoop,
	eSmfLocalPlayerModeSequence,
	eSmfLocalPlayerModeSequenceSort,
	eSmfLocalPlayerModeReverse,
	eSmfLocalPlayerModeReverseSort,	
	eSmfLocalPlayerModeRandom,
};

enum smf_local_playback_event_e {
	eSmfLocalPlayerEventNull = 0,
	eSmfLocalPlayerEventError,
	eSmfLocalPlayerEventMusicStart,
	eSmfLocalPlayerEventMusicStop,
	eSmfLocalPlayerEventMusicPause,
	eSmfLocalPlayerEventMusicResume,
	eSmfLocalPlayerEventMusicUnsupport,
	eSmfLocalPlayerEventUpdateProgress,
	eSmfLocalPlayerEventUpdateMeta,
	eSmfLocalPlayerEventUpdateMedia,
};

typedef struct smf_local_playback_info_t {
	uint8_t evt;///smf_local_playback_event_e
	uint8_t mode : 4;///smf_local_playback_mode_e
	bool mute : 1;
	uint8_t rev : 3;
	uint16_t volume;
	uint16_t index;///current index of player
	uint16_t count;///the count of playlist
	const char* current;///The filename of current music
	uint32_t fileIndex;///current index of playlist
	smf_meta_info_t meta;///the meta info of current music
	smf_error_t error;///error id
}smf_local_playback_info_t;

typedef struct smf_local_playback_cb_notify_t {
	uint32_t cycle;//ms
	void* priv;
	void (*cb)(void* priv, smf_local_playback_info_t*info);
}smf_local_playback_cb_notify_t;

typedef smf_cb_element_t smf_local_playback_cb_t;
typedef struct smf_local_playback_params_t {
	const char* url;///0,not set
	uint32_t timepoint;///1,not set
	uint32_t volume;///2,not set
	uint32_t mute;///3,not set
	void* meta;///4,not set
	smf_local_playback_cb_t* cb;///5,[in]the callback, use for algo/output
	uint32_t params[10];///6~15,[in]the customer params
}smf_local_playback_params_t;

enum {
	eSmfLocalPlayerFlags_Mute = 1u<<16,
	eSmfLocalPlayerFlags_Invoke = 1u<<17,
	eSmfLocalPlayerFlags_SaveConfigCycle = 1u<<18,
};

typedef struct smf_local_playback_open_t {
	const char* config;///[in]0, config file name
	const char* exts;///[in]1,filter the ext file name,eg.. mp3-aac-m4a
	uint32_t mode;///[in]2,smf_local_playback_mode_e
	uint32_t volume;///[in]3,base 32768	
	uint32_t revs;///[in]4,true/false		
	uint32_t flags;///[in]5,true/false,invoke start
	smf_local_playback_cb_notify_t* notify;///[in]6, the callback notify
	smf_local_playback_params_t* params;///[in]7, the pipeline params
	const char* script;///[in]the script of pipeline,default is null.
}smf_local_playback_open_t;

typedef struct smf_local_playlist_cb_t {
	void* priv;
	bool (*cb)(void* priv, uint32_t index, const char* filename);
}smf_local_playlist_cb_t;

typedef struct smf_local_playlist_t {
	char* buff;///[in]the buffer input 
	uint32_t buffsize;///[in]the size of buffer
	const char** filenames;///[out]the music on the playlist
	uint32_t count;///[out]the count of music on the playlist
}smf_local_playlist_t;

/*-----------------smf msg service API------------------*/
/**smf_msg_service_open
 * @script open msg service
 * @return true/false
 */
EXTERNC bool smf_msg_service_open(void);

/**smf_msg_service_close
 * @script close msg service
 * @return true/false
 */
EXTERNC bool smf_msg_service_close(void);

/*-----------------playback service ctrl API------------------*/
/**smf_local_playback_open
 * @script open playback service
 * @param[in] para[smf_local_playback_open_t]: open params
 * @return true/false
 */
EXTERNC bool smf_local_playback_open(smf_local_playback_open_t* para);

/**smf_local_playback_close
 * @script close playback service
 * @return true/false
 */
EXTERNC bool smf_local_playback_close(void);

/**smf_local_playback_start
 * @script start playing music
 * @param[in] is_continue[true/false]: [true]Use historical breakpoints 
 *				(fileinde and timestamp) to continue broadcasting
 * @return true/false
 */
EXTERNC bool smf_local_playback_start(bool is_continue);

/**smf_local_playback_stop
 * @script stop playing music
 * @return true/false
 */
EXTERNC bool smf_local_playback_stop(void);

/**smf_local_playback_pause
 * @script pause playing music 
 * @return true/false
 */
EXTERNC bool smf_local_playback_pause(void);

/**smf_local_playback_resume
 * @script resume playing music
 * @return true/false
 */
EXTERNC bool smf_local_playback_resume(void);

/*-----------------playlist load API------------------*/
/**smf_local_playback_folder_scan
 * @script scan music folder to create playlist.
 * @param[in] music_path: music folder path
 * @param[in] recursion[true/false]: scan recursion or not
 * @return true/false
 */
EXTERNC bool smf_local_playback_folder_scan(const char* music_path, bool recursion);

/**smf_local_playback_load_playlist
 * @script load playlist from string
 * @param[in] playlist: playlist string
 * @return true/false
 */
EXTERNC bool smf_local_playback_load_playlist(const char* playlist);

/**smf_local_playback_load_playlist_file
 * @script load playlist from file
 * @param[in] playlist_filename: playlist file name.
 * @return true/false
 */
EXTERNC bool smf_local_playback_load_playlist_file(const char* playlist_filename);

/*-----------------playback ctrl API------------------*/
/**smf_local_playback_set
 * @param[in] keys: the path
 * @param[in] vals: the values
 * @return true/false
 */
EXTERNC bool smf_local_playback_set(const char* keys, void* vals);

/**smf_local_playback_set
 * @param[in] keys: the path
 * @param[in] vals: the values
 * @return true/false
 */
EXTERNC bool smf_local_playback_get(const char* keys, void* vals);

/**smf_local_playback_set_volume
 * @script set player volume
 * @param[in] volume[0~32768]: volume value
 * @return true/false
 */
EXTERNC bool smf_local_playback_set_volume(uint32_t volume);

/**smf_local_playback_set_mute
 * @script set player mute
 * @param[in] mute[true/false]:
 * @return true/false
 */
EXTERNC bool smf_local_playback_set_mute(uint32_t mute);

/**smf_local_playback_forward
 * @script fast forward () second
 * @param[in] second:
 * @return true/false
 */
EXTERNC bool smf_local_playback_forward(uint32_t second);

/**smf_local_playback_rewind
 * @script fast rewind () second
 * @param[in] second:
 * @return true/false
 */
EXTERNC bool smf_local_playback_rewind(uint32_t second);

/**smf_local_playback_forward_percentage
 * @script fast forward ()%
 * @param[in] percentage[0~100]:
 * @return true/false
 */
EXTERNC bool smf_local_playback_forward_percentage(uint32_t percentage);

/**smf_local_playback_rewind_percentage
 * @script fast reward ()%
 * @param[in] percentage[0~100]:
 * @return true/false
 */
EXTERNC bool smf_local_playback_rewind_percentage(uint32_t percentage);

/**smf_local_playback_jump_to_timepoint
 * @script jump to timepoint of music
 * @param[in] second:
 * @return true/false
 */
EXTERNC bool smf_local_playback_jump_to_timepoint(uint32_t second);

/**smf_local_playback_jump_to_percentage
 * @script jump to ()% of music
 * @param[in] percentage[0~100]:
 * @return true/false
 */
EXTERNC bool smf_local_playback_jump_to_percentage(uint32_t percentage);

/*-----------------playlist ctrl API------------------*/
/**smf_local_playback_set_mode
 * @script set playlist mode
 * @param[in] mode[smf_local_playback_mode_e]:
 * @return true/false
 */
EXTERNC bool smf_local_playback_set_mode(enum smf_local_playback_mode_e mode);

/**smf_local_playback_next
 * @script switch to next music on the playlist
 * @return true/false
 */
EXTERNC bool smf_local_playback_next(void);

/**smf_local_playback_previous
 * @script switch to previous music on the playlist
 * @return true/false
 */
EXTERNC bool smf_local_playback_previous(void);

/**smf_local_playback_switch_to_index
 * @script switch to the index music on the playlist
 * @param[in] index: the index in playlist
 * @return true/false
 */
EXTERNC bool smf_local_playback_switch_to_index(uint32_t index);

/*-----------------playback info API------------------*/

/**smf_local_playback_get_info
 * @script get the current play info.
 * @return smf_local_playback_info_t
 */
EXTERNC smf_local_playback_info_t* smf_local_playback_get_info(void);

/**smf_local_playback_get_playlist
 * @script get the playlist string
 * @return string
 */
EXTERNC const char* smf_local_playback_get_playlist(void);

/**smf_local_playback_get_playlist_file
 * @script get the playlist file name
 * @return string
 */
EXTERNC const char* smf_local_playback_get_playlist_file(void);

/**smf_local_playback_get_playlist_with_callback
 * @script get the playlist with callback
 * @param[out] para[smf_local_playlist_cb_t]:
 * @return true/false
 */
EXTERNC bool smf_local_playback_get_playlist_with_callback(smf_local_playlist_cb_t* para);

/**smf_local_playback_get_playlist_with_buffer
 * @script get the playlist with buffer
 * @param [in] para[smf_local_playlist_t]:
 * @return [bool] true/false
 */
EXTERNC bool smf_local_playback_get_playlist_with_buffer(smf_local_playlist_t* para);

/**smf_local_playback_check
 * @script check the file can play or not.
 * @param[in] filename: file name.
 * @param[out] codecs: codec table.
 * @param[in] count: the table size.
 * @return true/false
 */
EXTERNC bool smf_local_playback_check(const char* filename, const char** codecs, int count);
#endif

