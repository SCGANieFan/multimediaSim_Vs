#pragma once
#include "Pipeline.h"
#include "SmfDef.h"
#include "SmfMsg.h"
namespace smf {
	namespace msg {
		class IMsgService;
		class MsgClientBase{
		public:
			MsgClientBase(Object* obj);
		protected:
			uint8_t _targetCpu = 0;
			Object* _this;
		protected:
			bool set(uint32_t key, void* val);
			bool get(uint32_t key, void* val)const;
		public:
			virtual bool init(IMsgService* svc, bool enable) { return true; }
			virtual Result Process(smf_msg_t*) { return Result::Other; }
			virtual bool Send(SmfMsg_t* msg) const ;
			IMsgService* svc() const;
		public:
			bool DefaultProcess(smf_msg_t*);
		public:
			void TargetCpu(int id) { _targetCpu = id; }
			bool Send(uint16_t cmd) const;
			bool Send(void* msg) const;
			template<class T>inline
			bool Send(uint16_t cmd, const T& data, uint8_t flags = 0) const { return Send(cmd, (T*)&data, sizeof(T), flags, 0); }
			template<class T>inline
			bool Send(uint16_t cmd, uint8_t cpuid, const T& data, uint8_t flags = 0) const { return Send(cmd, (T*)&data, sizeof(T), flags, cpuid); }
			bool Send(uint16_t cmd, void* data, int size, uint8_t flags, uint8_t cpuid) const;
			bool Response(bool rst, uint16_t cmd, uint16_t cpuid = 0) const;
			bool Response(bool rst, SmfMsg_t* msg) const;
			bool Response(bool rst, void* msg) const;
			template<class T>
			bool Response(bool rst, uint16_t cmd, T& para)  const { return Response(rst, cmd, &para, sizeof(T)); }
			bool Response(bool rst, uint16_t cmd, void* para, int size) const;
		public:
			bool SendWait(SmfMsg_t*, uint32_t timeout = 0xffffffff) const;
			bool SendWait(void* msg, uint32_t timeout = 0xffffffff) const;
			//
			template<class T>inline
			bool SendWait(T& msg, uint32_t timeout = 0xffffffff) const { return SendWait(&msg, sizeof(T), timeout); }
			bool SendWait(void* msg, int size, uint32_t timeout) const;
			//
			template<class T>inline
			bool SendWait(uint16_t cmd, T& data, uint32_t timeout = 0xffffffff) const { return SendWait(cmd, 0, &data, sizeof(T), 0, timeout); }
			template<class T>inline
			bool SendWait(uint16_t cmd, uint8_t cpuid, T& data, uint32_t timeout = 0xffffffff) const { return SendWait(cmd, cpuid, &data, sizeof(T), 0, timeout); }
			template<class T>inline
			bool SendWait(uint16_t cmd, uint8_t cpuid, uint8_t flags, T& data, uint32_t timeout = 0xffffffff) const { return SendWait(cmd, 0, &data, sizeof(T), &flags, timeout); }
			template<class T>inline
			bool SendWait(uint16_t cmd, uint8_t cpuid, T& data, uint8_t& flags, uint32_t timeout = 0xffffffff) const { return SendWait(cmd, 0, &data, sizeof(T), &flags, timeout); }
			bool SendWait(uint16_t cmd, uint8_t cpuid, void* data, int size, uint8_t* flags, uint32_t timeout) const;
		public:
			bool SendCallback(SmfMsg_t*, CbMsg cb, void* priv = 0) const;
			bool SendCallback(void* msg, CbMsg cb, void* priv = 0) const{ return SendCallback((SmfMsg_t*)msg, cb, priv); }
			//
			template<class T>inline
			bool SendCallback(T& msg, CbMsg cb, void* priv = 0) const { return SendCallback(&msg, sizeof(T), cb, priv); }
			bool SendCallback(void* msg, int size, CbMsg cb, void* priv = 0) const;
			//
			template<class T>inline
			bool SendCallback(uint16_t cmd, T& data, CbMsg cb, void* priv = 0) const { return SendCallback(cmd, 0, data, cb, priv); }
			template<class T>inline
			bool SendCallback(uint16_t cmd, uint8_t cpuid, T& data, CbMsg cb, void* priv = 0) const { return SendCallback(cmd, cpuid, &data, sizeof(T), 0, cb, priv); }
			bool SendCallback(uint16_t cmd, uint8_t cpuid, void* data, int size, uint32_t flags, CbMsg cb, void* priv = 0) const;
		public:
			enum {
				IsRunning = 1u << 0,
				SignalBlockingMask = 0x55aa0001,
			};
		};

		template<class Base>
		class TMsgClient
			: public MsgClientBase
			, public Base
		{
		public:
			TMsgClient() :MsgClientBase(this) { Base::_flags.Set(IS_MsgClient, true); }
			virtual bool set(uint32_t key, void* val)override { return MsgClientBase::set(key, val) || Base::set(key, val); }
			virtual bool get(uint32_t key, void* val)const override { return MsgClientBase::get(key, val) || Base::get(key, val); }
		};

		using IMsgClient = TMsgClient<Object>;
		using IMsgPipeline = TMsgClient<Pipeline>;
	}
}
#include "IMsgService.h"
//#include "IMsgPipeline.h"
