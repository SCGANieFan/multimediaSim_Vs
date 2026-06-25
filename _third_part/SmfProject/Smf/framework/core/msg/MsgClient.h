#pragma once
#include "MsgClient.msg.h"
#include "IMsgClient.h"
namespace smf {
	namespace msg {
		class MsgClient
			: public IMsgClient
		{
		public:
			MsgClient();
			virtual ~MsgClient();
		protected:
			virtual bool init(IMsgService* svc, bool enable)override;
			virtual Result Process(smf_msg_t* msg) override;
		protected:
			bool msgSet(smf_msg_t*);
			bool msgConfig(smf_msg_t*);
			bool msgInvoke(smf_msg_t*);
		public:
			bool MsgSet(uint8_t cpuid, const char* path, const void* data, int size, bool isval = false);
			bool MsgSet(uint8_t cpuid, const char* path, uint32_t vals);
			bool MsgConfig(uint8_t cpuid, const char* script, const void* params, int size);
			bool MsgInvoke(smf_cb_priv0 cb, void* priv);
			bool MsgInvokeFree(void* buff);
			bool MsgInvokeDelete(Object*obj);
		};
	}
}
