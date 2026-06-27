#pragma once
#include "IMsgClient.h"
#include "IPoolShared.h"
#include "MsgClientPool.msg.h"
namespace smf {
	namespace msg {
		namespace pool {
			namespace local {
				class MsgClientPool
					: public IMsgClient
					, public IPoolShared
				{
				protected:
					POOL _msg = POOL::null;
				protected:
					virtual void initialize(Reflection::item_t*) override;
				public:
					virtual bool init(IMsgService* svc, bool en) override;
					virtual Result Process(smf_msg_t* msg) override;
				public:
					virtual void* alloc(const char* name, void* buff, unsigned& size, unsigned align) override;
					virtual bool free(const char* name, void* buff) override;
				public:
					virtual bool IsLocal()const override { return false; }
				};
			}
		}
	}
}
