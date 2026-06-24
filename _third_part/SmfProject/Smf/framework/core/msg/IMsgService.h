#pragma once
#include "IMsgClient.h"
#include "IMsvc.h"
#include "SmfDef.h"
#include "SmfMsg.h"
#include <atomic>
#include <functional>
namespace smf {
	namespace msg {
		class IMsgService 
			: public TMsgClient<Service>
			, public api::IMsvc
		{
		protected:
			mutable std::atomic<uint8_t> _seqno = 0;
		public:
			using api::IMsvc::Register;
			using Service::Register;
			//virtual bool Invoke(const std::function<void()>&) = 0;
			//virtual bool Invoke(smf_cb_priv0 cb, void* priv) = 0;
			//bool InvokeDelete(Object* obj);
			virtual bool Receive(smf_msg_t* msg) override { return Process(msg) == Result::True; }
			//bool Receive(smf_msg_t& msg) { return Process(&msg) == Result::True; }
			uint8_t SeqNO() const { auto seqno = _seqno++; return seqno ? seqno : _seqno++; }
		};
	}
}


