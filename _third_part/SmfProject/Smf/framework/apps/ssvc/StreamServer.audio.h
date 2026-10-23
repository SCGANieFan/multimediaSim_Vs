#pragma once
#include "StreamServer.h"
#include "StreamDefine.h"
#include "mutex.h"
namespace smf {
	namespace ssvc {
		class AudioServer
			: public StreamServer
		{
		private:
			using Base = StreamServer;
		public:
			AudioServer(uint64_t id, eStream msg, uint64_t name);
			virtual ~AudioServer();
		//protected:
			//mutex _imtx;
			//mutex _omtx;
		protected:
			uint64_t _name = 0;
			uint32_t _rate = 0;
			uint8_t _ch = 0;
			uint8_t _bits = 0;
			uint16_t _fsize = 0;
			const char* _script = 0;
			uint16_t _volume = 32768;
			bool _mute = false;
			int8_t _srcCnt = 0;
			int8_t _sinkCnt = 0;
			int8_t _otherCnt = 0;
		protected:
			virtual bool set(uint32_t key, void* val)override;
			virtual bool add(MsgAdd_t& val, streams_t& item, const char* script)override;
			virtual bool remove(MsgRemove_t& val, streams_t& item)override;
		protected:
			bool createScript();
		public:
			template<uint64_t id, uint16_t msg, uint64_t name>
			static Object* Create() {
				return new AudioServer(id, (eStream)msg, name);
			}
		};
	}
}
