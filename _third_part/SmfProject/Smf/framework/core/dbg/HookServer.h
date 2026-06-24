#pragma once
#include "HookDefine.h"
#include "IMsgClient.h"
#include "ShmFifo.h"
#include "IO.h"
#include "mutex.h"
namespace smf {
	namespace msg {
		class HookServer :public IMsgClient {
		public:
			HookServer();
			virtual ~HookServer();
		protected:
			smf_hook_cb _cbhook = 0;
			void* _cbpriv = 0;
			mutex _mtx;
			uint64_t _flags64 = 0;
			const char* _filePattern = 0;
			uint32_t _interval = 10;
		public:
			virtual Result Process(smf_msg_t* msg) override;
			virtual bool init(IMsgService* svc, bool enable) override;
		protected:
			virtual bool set(uint32_t keys, void* vals)override;
			virtual char* print(char* ptr,char*end)const override;
		protected:
			bool callback();
		public:
			bool msgHook(smf_msg_t*msg);
			bool Start(uint32_t pointer);
			bool Stop(uint32_t pointer);
			using IMsgClient::Register;
			bool Register(smf_hook_cb cbhook, void* priv);
			bool Hook(uint32_t pointer, bool start);
		public:
			typedef struct item_t {
				IO* _io;
				uint32_t _datacnt;
				uint32_t _datasize;
				const char* _path;
				uint8_t _cpuid;
				uint8_t _rev8;
				uint16_t _rev16;
			}item_t;
		protected:
			item_t _items[64];
		public:
			bool Config(const char*);
		};
	}
}