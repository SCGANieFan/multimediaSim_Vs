#pragma once
#include "IMsgClient.h"
#include "MsgClientPool.msg.h"
#include "MemoryNamed.h"
namespace smf {
	namespace msg {
		namespace pool {
			namespace remote {
				class MsgClientPool
					:public IMsgClient
				{
				public:
					MsgClientPool();
				protected:
					virtual void initialize(Reflection::item_t*) override;
				public:
					virtual bool set(uint32_t keys, void* vals)override;
					virtual bool init(IMsgService* svc, bool en) override;
					virtual Result Process(smf_msg_t* msg) override;
				protected:///vad
					bool msgAlloc(smf_msg_t*);
					bool msgFree(smf_msg_t*);
				protected:
					MemoryNamed _named;
					int _memCnt = 128;
					IPool* _pool = 0;
					POOL _msg = POOL::null;
				};
			}
		}
	}
}
