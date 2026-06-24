#pragma once
#include "HookDefine.h"
#include "IMsgClient.h"
#include "ShmFifo.h"
#include "IHook.h"
#include "mutex.h"
namespace smf {
	namespace msg {
		class HookClient
			: public IMsgClient
		{
		public:
			HookClient();
			virtual ~HookClient();
		public:
			bool Hook(Frame&);
		protected://IMsgClient
			virtual bool init(IMsgService* svc, bool enable)override;
			virtual Result Process(smf_msg_t* msg) override;
		protected:
			virtual bool set(uint32_t keys, void* vals)override;
			virtual char* print(char* ptr,char*end)const override;
		protected:
			bool msgHook(smf_msg_t*);
			bool msgStart(uint32_t pointer,const char* path);
			bool msgStop(uint32_t pointer, const char* path);
		protected:
			ShmFifo* _shm = 0;
			mutable mutex _mtx;
			uint64_t _flags64 = 0;
			uint32_t _shmSize = 1024*64;
			uint32_t _shmCount = 64;
			uint32_t _delay = 100;
			char _shmname[16];
			uint8_t _shmIndex = 0;
		public:
			class Item_t 
				:public IHook 
			{
			public:
				HookClient* _client = 0;
				uint32_t _pointer = 0;
				uint32_t _datacnt = 0;
				uint32_t _datasize = 0;
			public:
				virtual bool Hook(Frame*&)override;
			};
		public:
			Item_t _items[64];
		public:
			bool Config(const char*);
		};
	}
}
