#pragma once
#include "smf_common.h"
#include "SmfICS.msg.h"
#include "SmfICS.time.h"
#include "SmfICS.stream.h"
#include "SmfICS.mmgr.h"
//ics: inter core shared-block
#define SMF_ICS_MASK 0xbe5be5be
#define SMF_ICS_VERSION 0x20260414
namespace smf {
	namespace ics {
		typedef struct smf_ics_t {//align to 64bits
			uint32_t magic;
			uint32_t size;
			uint32_t version;
			uint8_t lock;
			uint8_t flags;
			uint8_t power;//bits[8]
			uint8_t ready;//bits[8]
			uint64_t names[8];//cpuname
			uint32_t hooks[8];
			uint16_t sysfreq[8];
			//
			smf_fifo_t log[8];
			uint32_t display[4][4];
			smf_pair32_t script;
			//
			ics::localtime_t time;
			ics::msg_t msg;
			ics::stream_t stream;
			ics::mmgr_t mmgr;
			//
			uint8_t cpus[8];//refs
			//
			uint32_t size_end;
			uint32_t magic_end;
		}smf_ics_t;//always-on memory
	}
}