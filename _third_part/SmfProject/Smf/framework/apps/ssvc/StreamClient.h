#pragma once
#include "StreamDefine.h"
#include "StreamServer.h"
#include "StreamConfig.h"
#include "SimpleArray.h"
#include "SimpleTable.h"
#include "IMsgClient.h"
#include "SmfLayout.h"
#include "TaskScheduler.h"
#include "unique_ptr.h"
#include "mutex.h"
#include <atomic>
#include "smf_stream_api.h"
namespace smf {
	namespace ssvc {
		class StreamClient :public msg::IMsgClient {
		public:
			StreamClient();
			virtual ~StreamClient();
		public:
			using Base = msg::IMsgClient;
			friend class StreamServer;
			friend class AudioServer;
		protected:
			virtual bool init(msg::IMsgService* svc, bool enable)override;
			virtual Result Process(smf_msg_t*)override;
		protected:
			virtual bool set(uint32_t key, void* val)override;
		public:
			virtual bool Send(SmfMsg_t* msg) const override;
			bool MessageCallback(smf_stream_message_callback_e id, smf_stream_message_cb callback, void* priv, bool invoke = false);
		public:
			bool Start(uint32_t stream
				, const void* param, uint32_t size
				, uint32_t timeout = 0
				, bool string = false
			);
			bool Stop(uint32_t stream, bool force = false);
			bool Pause(uint32_t stream);
			bool Resume(uint32_t stream);
		public:
			bool Start(const char* script);
			bool Start(uint32_t stream, const char* param, uint32_t timeout = 0);
		public:
			uint32_t GetFree(uint32_t begin, uint32_t end)const;
			uint8_t GetSeqNO(uint32_t stream)const;
			bool Check(uint32_t stream)const;
		public:
			using Base::Set;
			using Base::Get;
		protected:
			bool Set(uint32_t stream, uint32_t cpuid, const char* path, void* data, uint32_t size);
			//bool Get(uint32_t stream, uint32_t cpuid, const char* path, smf_cb_data cb, void* priv);
			//bool Get(uint32_t stream, uint32_t cpuid, const char* path, void* data, uint32_t& size, uint32_t timeout = -1);
		public:
			bool Set(uint32_t stream, uint32_t setid, void* data, uint32_t size);
			bool Get(uint32_t stream, uint32_t setid, smf_cb_data cb, void* priv);
			bool Get(uint32_t stream, uint32_t setid, void* data, uint32_t& size, uint32_t timeout);
		protected:
			bool Set32(uint32_t stream, uint32_t cpuid, const char* path, uint32_t value);
			//bool Get32(uint32_t stream, uint32_t cpuid, const char* path, uint32_t& value, uint32_t timeout = -1);
		public:
			bool Set32(uint32_t stream, uint32_t setid, uint32_t value);
			bool Get32(uint32_t stream, uint32_t setid, uint32_t& value, uint32_t timeout);
		public:
			bool SetVolume(uint32_t stream, uint32_t volmue);
			bool SetMute(uint32_t stream, bool mute);
			bool SetVolumeReduce(uint32_t stream, bool reduce);
			bool SetMuteForce(uint32_t stream, bool force);
		public:
			bool Seek(uint32_t stream, uint32_t timepoint);
		public:
			bool Push(uint32_t stream, smf_frame_t* frm);
			bool Pull(uint32_t stream, smf_frame_t*& frm);
		public:
			bool Hook(uint32_t pointer, bool start);
			bool HookCallback(smf_hook_cb callback, void* priv);
		protected:
			bool Job(smf_msg_t* msg);
			bool Notify(smf_msg_t* msg);
			bool Device(smf_msg_t* msg);
			bool GetResponse(smf_msg_t* msg);
			bool AddResponse(smf_msg_t* msg);
			bool RemoveResponse(smf_msg_t* msg);
			bool StopWithSeqNO(uint8_t stream, uint8_t seqno);
		protected:
			bool SetJob(bool enable);
			bool Set(uint32_t stream, uint32_t cpuid, uint32_t setid, const char* path, void* data, uint32_t size, bool is_value);
			bool Get(uint32_t stream, uint32_t cpuid, uint32_t setid, const char* path, smf_cb_data cb, void* priv, void* buff, uint32_t buff_max, uint32_t timeout);
			bool Notify(uint8_t msgid, uint8_t stream, uint8_t seqno, void* data = 0, int size = 0);
			bool NotifyCallback(smf_stream_message_t& msg);
		protected:
			bool PowerOn(uint8_t cpuid);
			bool PowerOff(uint8_t cpuid);
		protected:
			mutable mutex _mtx;
			StreamServer* _servers[2];
			struct notify_t {
				smf_stream_message_cb func;
				void* priv;
			};
			notify_t _notifys[SMF_STREAM_MESSAGE_CALLBACK_end];
			StreamConfig _cfg;
			struct stream_t {
				int8_t cnt : 3;
				uint8_t rev : 1;
				int8_t idx : 3;
				bool result : 1;
			};
			stream_t _streams[eStreamMax];
			uint64_t _msgStack[512 / 8];
			enum class eFlagsExt8 {
				NotifyInvoke = 1u << 8,
			};
		public:
			bool Set(StreamServer* server, eStream es);
			StreamConfig& GetConfig()const;
		};
	}
}
