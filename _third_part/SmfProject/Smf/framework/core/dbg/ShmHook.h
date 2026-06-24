#pragma once
#include "ShmFifo.h"
#include "IHook.h"
#include "mutex.h"
namespace smf {
	class ShmHook 
		: public IHook
	{
	public:
		bool init(uint8_t pointer, uint32_t buffmax, uint32_t fifomax, mutex* mtx);
	public:
		ShmHook(uint8_t pointer, uint32_t buffmax, uint32_t fifomax, mutex* mtx);
		ShmHook(uint8_t pointer, ShmFifo* shm, mutex* mtx);
		virtual ~ShmHook();
	public:
		mutex* _mtx = 0;
		ShmFifo* _shm = 0;		
		uint32_t _datacnt = 0;
		uint32_t _datasize = 0;
		uint8_t _pointer = 0;
		bool _shmDelete = false;
	public:
		virtual bool Hook(Frame*&)override;
	};
}
