#pragma once
#include "smf_common.h"
#include "StreamDefine.h"
#include "Locker.h"
namespace smf {
	namespace ssvc {
		class StreamConfig {
		public:
			struct stream_t {
				uint8_t cpu0 : 3;
				uint8_t cpu1 : 3;
				uint8_t seqno : 2;
				uint8_t reentry : 1;
				uint8_t msgid : 3;
				uint8_t refs : 4;
			};
			struct cfg_t {
				uint64_t stream_name[eStreamMax];
				stream_t stream_item[eStreamMax];
				uint32_t set_path[eSetidMax];//const char*
				uint8_t set_cpuid[eSetidMax];
				uint8_t lock;
			};
		public:
			StreamConfig();
		protected:
			cfg_t* _cfg;
		public:
			bool set(uint32_t key, void* val);
			operator bool()const { return _cfg; }
			cfg_t* config() const { return _cfg; }
			stream_t* streams(uint8_t stream) const { return &_cfg->stream_item[stream]; }
		protected:
			bool parseStream(const char*);
			bool parseSet(const char*);
			//bool parseHook(const char*);
			//bool parseNotify(const char*);
		public:
			bool CheckStream(uint32_t stream)const;
			uint8_t GetStreamMax()const;
			uint64_t GetName64(uint32_t stream)const;
			const char* GetName(uint32_t stream)const;
			uint32_t GetMsgID(uint32_t stream)const;
			uint8_t GetCpuID(uint32_t stream, uint8_t index)const;
			uint8_t GetReEntry(uint32_t stream)const;
			uint32_t GetStream(uint64_t name)const;
			uint32_t GetStream(const char* name)const;
		public:
			uint8_t GetSeqNO(uint32_t stream)const;
			uint8_t GetRefs(uint32_t stream)const;
			void SeqNOIncrease(uint32_t stream);
			void RefsIncrease(uint32_t stream);
			void RefsReduce(uint32_t stream);
			void RefsClear(uint32_t stream);
		public:
			bool CheckSetID(uint32_t setid)const;
			const char* GetSetPath(uint32_t setid)const;
			uint8_t GetSetCpuID(uint32_t setid)const;
		};

		class Locker : public smf::Locker {
		public:
			Locker(uint32_t timeout = 0);
		};
	}
}
