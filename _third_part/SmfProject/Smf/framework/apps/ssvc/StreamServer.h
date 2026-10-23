#pragma once
#include "SmfPortingDevice.h"
#include "StreamConfig.h"
#include "IMsgClient.h"
#include "SimpleTable.h"
#include "SimpleArray.h"
#include "StreamDefine.h"
#include <atomic>
namespace smf {
	namespace ssvc {
		class StreamServer
			: public msg::IMsgPipeline
		{
		private:
			using Base = msg::IMsgPipeline;
			friend class StreamClient;
		public:
			StreamServer(uint64_t id, eStream msg);
			virtual ~StreamServer();
		protected:
			eStream _msg;
			uint8_t _add_count = 0;
		protected:
			SimpleArrayC<const char*, eStreamMax> _pipelines;
			SimpleTableC<const char*, 32> _params;
			SimpleTable<const char*>* _params_add = 0;
			struct streams_t {
				Object* obj;
				uint8_t cpuid;
				uint8_t seqno;
				uint8_t refs;
				std::atomic_bool lock;
			};
			streams_t _streams[eStreamMax];
			StreamConfig _cfg;
		protected:
			virtual bool stop()override;
			virtual bool processMessage(smf_message_t&)override;
			virtual bool set(uint32_t key, void* val)override;
			virtual ParamTable* GetParamTable() const override;
			virtual bool add(MsgAdd_t& val, streams_t& item, const char* script);
			virtual bool remove(MsgRemove_t& val, streams_t& item);
		public:
			virtual bool init(msg::IMsgService* svc, bool enable)override;
			virtual Result Process(smf_msg_t*)override;
		public://msg
			bool Notify(Object* obj, eNotify notify, void* data = 0, uint32_t size = 0);
			bool Device(uint8_t device, uint8_t op, MediaDeviceCfg_t* cfg);
		public:
			using Base::Add;
			using Base::Remove;
			using Base::Set;
			using Base::Get;
		public:
			virtual bool Add(smf_msg_t* msg);
			virtual bool Remove(smf_msg_t* msg);
			bool Set(smf_msg_t* msg);
			bool Get(smf_msg_t* msg);
			//bool Hook(smf_msg_t* msg);
			bool Process(uint8_t stream, smf_frame_t* frm);
		public:
			template<uint64_t id, uint16_t msg>
			static Object* Create() {
				return new StreamServer(id, (eStream)msg);
			}
		protected:
			int getStream(Object* obj);
		};
	}
}
