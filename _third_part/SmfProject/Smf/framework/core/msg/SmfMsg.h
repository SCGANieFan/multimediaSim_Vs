#pragma once
#include "smf_msg.h"
#include "SmfMsg.def.h"
namespace smf {
	namespace msg {
		enum class EXT {
			null = 0,
			callback,
			wait,
		};
		template<class ...TS>
		class TClass :public TS...{};
		//
		using MsgExt = SmfMsgResponse_t;
		template<class ...T>
		using TMsgExt = TClass<MsgExt, T...>;
		//
		template<class ...T>
		using TMsg = TClass<SmfMsg_t, T...>;
		template<int X>
		class XMsg :public SmfMsg_t {
		public:
			char _buff[X - sizeof(SmfMsg_t)];
		};
		template<class ...T>
		using TMsgResponse = TClass<SmfMsgResponse_t, T...>;
		////////////////////////////////////
		//msg ext with callback
		struct ExtCallBack_t {
			uint64_t callback;// bool (*callback)(smf_msg_t* msg);
			uint64_t priv;//void*
		};
		using MsgExtCallback = TMsgExt<ExtCallBack_t>;
		template<class ...T>
		using TMsgExtCallback = TClass<MsgExtCallback,T...>;
		//////////////////////////////////
		//msg ext with wait
		struct ExtWait_t {
			uint64_t wait;
			uint64_t msg;//TMsgExt<ExtWait_t>* msg;
		};
		using MsgExtWait = TMsgExt<ExtWait_t>;
		template<class ...T>
		using TMsgExtWait = TClass<MsgExtWait,T...>;
		//msg ext with local&remote
		struct ExtLocalRemote_t {
			uint64_t local;//void*
			uint64_t remote;//void*
		};
		//
		bool MsgIsResponse(SmfMsg_t* msg);
		void MsgToResponse(SmfMsg_t* msg);
		ExtCallBack_t* MsgGetExtCallback(SmfMsg_t* msg);
		ExtWait_t* MsgGetExtWart(SmfMsg_t* msg);
		ExtLocalRemote_t* MsgGetExtLocalRemote(SmfMsg_t* msg);
		int MsgGetBodyOffset(SmfMsg_t* msg);
		template<class T>
		T* MsgGetBody(SmfMsg_t* msg) {
			return (T*)((char*)msg + MsgGetBodyOffset(msg));
		}
		//
		bool MsgRouteSet(uint8_t cpuid, uint16_t msgid);
		bool MsgRouteSet(uint8_t cpuid, uint16_t msgid_begin, uint16_t msgid_end);
		bool MsgRouteRegister(uint16_t msgid);
		bool MsgRouteRegister(uint16_t msgid_begin, uint16_t msgid_end);
		template<class T>
		bool MsgRouteRegister(T msgid) { return MsgRouteRegister((uint16_t)msgid); }
		template<class T>
		bool MsgRouteRegister(T msgid_begin, T msgid_end) { 
			return MsgRouteRegister((uint16_t)msgid_begin,(uint16_t)msgid_end);
		}
		uint8_t MsgRouteGet(uint16_t msgid);
	}
}
