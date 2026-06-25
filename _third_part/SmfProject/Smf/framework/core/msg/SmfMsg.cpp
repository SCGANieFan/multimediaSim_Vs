#include "SmfMsg.h"
#include "smf_debug.h"
#include "SmfMsg.def.h"
#include "SmfICS.h"
#include "SmfPorting.h"
namespace smf {
	namespace msg {
		bool MsgIsResponse(SmfMsg_t* msg) {
			return msg->cmd & 1; 
		}
		void MsgToResponse(SmfMsg_t* msg) {
			msg->cmd |= 1; 
		}
		int MsgGetBodyOffset(SmfMsg_t* msg) {
			int offset = sizeof(SmfMsg_t);
			if (MsgIsResponse(msg))offset = sizeof(SmfMsgResponse_t);
			
			switch ((EXT)(msg->ext)) {
			case EXT::callback: offset = sizeof(MsgExt) + sizeof(ExtCallBack_t); break;
			case EXT::wait: offset = sizeof(MsgExt) + sizeof(ExtWait_t); break;
			default:break;
			}

			if (msg->localRemote)offset += sizeof(ExtLocalRemote_t);
			return offset;
		}

		ExtCallBack_t* MsgGetExtCallback(SmfMsg_t* msg) {
			return (ExtCallBack_t*)(((EXT)msg->ext == EXT::callback) ? (msg + 1) : 0);
		}
		ExtWait_t* MsgGetExtWart(SmfMsg_t* msg) {
			return (ExtWait_t*)(((EXT)msg->ext == EXT::wait) ? (msg + 1) : 0);
		}
		ExtLocalRemote_t* MsgGetExtLocalRemote(SmfMsg_t* msg) {
			returnIf(0, !msg->localRemote);
			int offset = sizeof(SmfMsg_t);
			if (MsgIsResponse(msg))offset = sizeof(SmfMsgResponse_t);
			switch ((EXT)(msg->ext)) {
			case EXT::callback: offset = sizeof(MsgExt) + sizeof(ExtCallBack_t); break;
			case EXT::wait:offset = sizeof(MsgExt) + sizeof(ExtWait_t); break;
			default:break;
			}
			return (ExtLocalRemote_t*)((char*)msg + offset);
		}
		//
		bool MsgRouteSet(uint8_t cpuid, uint16_t msgid) {
			auto tbl = ics::GetMsgRoute();
			//dbgTestPXL("%p,%u,%03x", tbl, cpuid, msgid);
			returnIfErrC(false, !tbl);
			returnIfErrC(false, msgid >= SMF_MSG_MAX);
			returnIfErrC(false, cpuid >= SMF_CPU_ID_END);
			auto idx = msgid >> 1;
			uint8_t msk = (idx & 1u) ? 0xf0u : 0x0fu;
			uint8_t val = (idx & 1u) ? cpuid : (cpuid << 4);
			if (tbl[idx >> 1] & ~msk) {
				dbgWarnPXL("route change:%02x,%02x<-%02x", msgid, cpuid, (idx & 1) ? (tbl[idx >> 1] >> 4) : (tbl[idx >> 1] & 0x0f));
			}
			tbl[idx >> 1] = val | (tbl[idx >> 1] & msk);
			return true;
		}
		bool MsgRouteSet(uint8_t cpuid, uint16_t msgid_begin, uint16_t msgid_end) {
			auto tbl = ics::GetMsgRoute();
			dbgTestPXL("%p,%u,%03x~%03x", tbl, cpuid, msgid_begin, msgid_end);
			returnIfErrC(false, !tbl);
			returnIfErrC(false, msgid_end >= SMF_MSG_MAX);
			returnIfErrC(false, cpuid >= SMF_CPU_ID_END);
			auto idx0 = msgid_begin >> 1;
			auto idx1 = msgid_end >> 1;
			if (idx0 & 1) {
				//tbl[idx0 >> 1] = (tbl[idx0 >> 1] & 0xf0) | cpuid;
				MsgRouteSet(cpuid, idx0 << 1);
				idx0++;
			}
			if (idx1 & 1) {
				//tbl[idx1 >> 1] = (tbl[idx1 >> 1] & 0xf0) | cpuid;
				MsgRouteSet(cpuid, idx1 << 1);
				idx1--;
			}
			auto val = cpuid | (cpuid << 4);
			for (auto idx = (idx0 >> 1); idx < (idx1 >> 1); idx++) {
				if (tbl[idx]) {
					dbgWarnPXL("route change:%02x,%02x<-%02x", idx << 2, val, tbl[idx]);
				}
				tbl[idx] = val;
			}
			return true;
		}
		bool MsgRouteRegister(uint16_t msgid) {
			return MsgRouteSet(get_cpu_idx(), msgid);
		}
		bool MsgRouteRegister(uint16_t msgid_begin, uint16_t msgid_end) {
			return MsgRouteSet(get_cpu_idx(), msgid_begin, msgid_end);
		}
		uint8_t MsgRouteGet(uint16_t msgid) {
			auto tbl = ics::GetMsgRoute();
			returnIfErrC(false, !tbl);
			returnIfErrC(0, msgid>= SMF_MSG_MAX);
			auto idx = msgid >> 1;
			auto val = tbl[idx >> 1];
			return (idx & 1) ? (val & 0x0f) : (val >> 4);
		}
	}
}
