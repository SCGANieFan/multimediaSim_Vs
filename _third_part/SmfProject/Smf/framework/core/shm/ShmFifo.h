#pragma once
#include "Frame.h"
namespace smf{
	class ShmFifo {
	public:
		bool Valid(bool cacheable)const;
		bool WriteValid()const;
		bool ReadValid()const;
		bool MediaValid()const;
		bool Full(bool isread = false)const;
		bool Empty(bool isread = false)const;
		uint32_t Size()const;
		uint32_t GetFifoMax()const;
		uint32_t GetBuffMax()const;
		uint32_t GetFifoUsed(bool isread = false)const;
		uint32_t GetFifoLeft(bool isread = false)const;		
		uint32_t GetBuffUsed(bool isread = false)const;
		uint32_t GetBuffLeft(bool isread = false)const;		
	public:
		bool Initialize(int total, int data_size, int fifo_max, bool cacheable, bool nomedia = false, bool reset = false, uint8_t align = 0);
		bool InitWrite(bool cacheable, bool nocopy = false, bool isring = false);
		bool InitRead(bool cacheable, bool nocopy = false, bool partread = false);
		bool DeInitialize();
	public:
		bool Write(Frame&);
		bool Read(Frame&);
		bool Sync(int offset = 0);
	public:
		struct mem_t {
			void* buff;
			uint32_t size;
			uint64_t priv[2];
		};
		bool SendPrepare(mem_t& mem, uint32_t size);
		bool Send(mem_t& mem, Frame& frm);
	public:
		bool Write(IMedia* media);
		IMedia* Read(uint32_t timeout = 0);
	public://sharedinfo
		enum sinfo_e {
			eSInfo_forward_flags32 = 0x00,
			eSInfo_volume,
			eSInfo_mute,
			eSInfo_prompt,
			eSInfo_fade,
			eSInfo_forward_flags_set = 0x20,
			eSInfo_forward_flags_clear = 0x40,

			eSInfo_backward_flags32 = 0x60,
			eSInfo_backward_flags_set = 0x80,
			eSInfo_backward_flags_clear = 0xa0,
		};
		bool SetSInfo(sinfo_e keys, uint32_t vals);
		bool GetSInfo(sinfo_e keys, uint32_t& vals);
	public://sharedinfo
		bool SetVolume(uint16_t volume);
		bool SetMute(bool mute);
	public:
		char* Print(char* ptr, char* end)const;
	public:
		static uint32_t GetSize(int buff_max, int fifo_max);
		static ShmFifo* Open(const char* shm_name, int buff_max, int fifo_max, int shared_index = 0, bool reset = false, uint8_t align = 0);
		static ShmFifo* Open(const char* shm_name, uint32_t timeout, int shared_index = 0);
		static bool Close(ShmFifo* shm, int shared_index = 0);
	public:
		enum class set_e {
			begin = 0,
			noForward,
			noBackward,
			max,
		};
		bool Set(set_e key, void* value);
		bool SetWrite(set_e key, void* value);
		bool SetRead(set_e key, void* value);
	};
}
