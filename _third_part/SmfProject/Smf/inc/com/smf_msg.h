#pragma once
#include "stdint.h"
#include "stdbool.h"
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define SMF_MSG_VERSION 2023082800
///smf msg message header
typedef struct SmfMsg_t {
	union {
		uint64_t head;
		struct {
			uint16_t cmd : 12;
			uint8_t cpusrc : 4;
			uint16_t size : 12;
			uint8_t cpudst : 4;

			uint8_t seqno;
			uint8_t svcid;
			uint8_t flags;
			union {
				uint8_t flagsExt;
				struct {
					uint8_t ext : 4;
					uint8_t localRemote : 1;
					uint8_t result : 1;
					uint8_t string : 1;
					uint8_t rev1 : 1;
				};
			};
		};
	};
} smf_msg_t, SmfMsg_t, smf_msg_response_t, SmfMsgResponse_t;
//

enum smf_cpu_id_e{
	SMF_CPU_ID_BEGIN = 0X00,
	SMF_CPU_DEFAULT = 0X00,
	SMF_CPU_AP = 0x01,
	SMF_CPU_CP = 0x02,
	SMF_CPU_CP_2 = 0x03,
	SMF_CPU_DSP = 0x04,
	SMF_CPU_DSP_2 = 0x05,
	SMF_CPU_CP_3 = 0x06,
	SMF_CPU_CP_4 = 0x07,
	SMF_CPU_ID_END,
};

typedef bool(*CbMsg)(smf_msg_t*);
typedef bool(*CbMsgPriv)(smf_msg_t*,void* priv);
typedef bool(*CbMsgSizePriv)(smf_msg_t*,uint32_t size,void* priv);

