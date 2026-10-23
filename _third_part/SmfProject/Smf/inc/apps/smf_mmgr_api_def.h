#pragma once
#include "smf_mmgr_api.h"

#define SMF_SCENE_Record "record"
#define SMF_SCENE_Wakeup "wakeup"
#define SMF_SCENE_Detect "detect"
#define SMF_SCENE_Music "music"
#define SMF_SCENE_Notify "notify"
#define SMF_SCENE_Prompt "prompt"
#define SMF_SCENE_SCO "sco"
#define SMF_SCENE_Ring "ring"
#define SMF_SCENE_Alarm "alarm"

#define SMF_APP_AudioPlayerURL "audio_player_url"
#define SMF_APP_AudioPlayerKFifo "audio_player_kfifo"
#define SMF_APP_AudioPlayerPush "audio_player_push"
#define SMF_APP_AudioPlayerCallback "audio_player_callback"
#define SMF_APP_AudioRecordURL "audio_record_url"
#define SMF_APP_AudioRecordPull "audio_record_pull"
#define SMF_APP_AudioRecordCallback "audio_record_callback"
#define SMF_APP_AudioSCO "audio_sco"
#define SMF_APP_AudioVADURL "audio_vad_url"
#define SMF_APP_AudioBridge "audio_bridge"
#define SMF_APP_AudioWakeup "audio_wakeup"
#define SMF_APP_AudioA2dpSink "audio_a2dp_sink"
#define SMF_APP_VideoPlayerURL "video_player_url"
#define SMF_APP_VideoRecordURL "video_record_url"
#define SMF_APP_AVPlayerURL "av_player_url"
#define SMF_APP_AVRecordURL "av_record_url"
#define SMF_APP_AVRtspServer "av_rtsp_server"
#define SMF_APP_AVRtspClient "av_rtsp_client"
#define SMF_APP_AVRtspPusher "av_rtsp_pusher"

#define SMF_AUDIO_OUTPUT_SPK0 "spk0"
#define SMF_AUDIO_OUTPUT_SPK1 "spk1"
#define SMF_AUDIO_OUTPUT_SPK2 "spk2"
#define SMF_AUDIO_OUTPUT_A2DP0 "a2dpdn0"
#define SMF_AUDIO_OUTPUT_A2DP1 "a2dpdn1"
#define SMF_AUDIO_OUTPUT_A2DP2 "a2dpdn2"

#define SMF_AUDIO_INPUT_MIC0 "mic0"
#define SMF_AUDIO_INPUT_MIC1 "mic1"
#define SMF_AUDIO_INPUT_MIC2 "mic2"
#define SMF_AUDIO_INPUT_MIC3 "mic3"

typedef struct smf_media_prepare_t {
	union {
		struct {
			uint32_t volume;
			uint32_t mute;
			union {
				union {
					struct {
						void* rev;
						const char* url;
						const char* media;
					}url;
					struct {
						const char* media;
						void* kfifo;
					}kfifo;
					struct {
						const char* media;
					}push;
					struct {
						smf_media_frame_cb cb;
						void* priv;
						const char* media;
					}callback;
					struct {
						const char* device;
						const char* media;
					}device;
					struct {
						int num;
						int len;
					}tone;
				}player;
				union {
					struct {
						const char* url;
						const char* format;
						const char* encode;
					}url;
					struct {
						const char* format;
						const char* encode;
					}pull;
					struct {
						smf_media_frame_cb cb;
						void* priv;
						const char* format;
						const char* encode;
					}callback;
					struct {
						const char* device;
						const char* format;
					}device;
					struct {
						void* kfifo;
						const char* format;
						const char* encode;
					}kfifo;
				}record;
				union {
					struct {
						const char* src;
						const char* format;
						const char* dst;
					}transfer;
				}bridge;
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
				union {
					struct {
						const char* url;
						const char* format;
						const char* encode;
					}url;
					struct {
						const char* format;
						const char* encode;
					}pull;
					struct {
						smf_media_frame_cb cb;
						void* priv;
						const char* format;
						const char* encode;
					}callback;
				}vad;
				struct {
					uint32_t stream;
					const char* param;
				}wakeup;
				union {
					struct {
						const char* fmt;
						const char* enc;
					}sink;
					struct {
						const char* media;
					}source;
				}a2dp;
			};
		}audio;
		union {
			union {
				struct {
					void* rev;//unused
					const char* url;
					const char* media;
				}url;
				struct {
					const char* media;
				}push;
				struct {
					smf_media_frame_cb cb;
					void* priv;
					const char* media;
				}callback;
			}player;
			union {
				struct {
					const char* url;
					const char* encode;
					const char* muxer;
				}url;
				struct {
					const char* encode;
				}pull;
				struct {
					smf_media_frame_cb cb;
					void* priv;
					const char* encode;
				}callback;
			}record;
		}video;
		union {
			union {
				struct {
					void* rev;//unused
					uint32_t volume;
					uint32_t mute;
					const char* url;
				}url;
			}player;
			union {
				struct {
					const char* url;
					const char* aformat;
					const char* aencode;
					const char* vencode;
				}url;
			}record;
			union {
				struct {
					const char* param;
				}server;
				struct {
					const char* url;
				}client;
				struct {
					const char* url;
				}pusher;
			}rtsp;
		}av;
	};
}smf_media_prepare_t;

