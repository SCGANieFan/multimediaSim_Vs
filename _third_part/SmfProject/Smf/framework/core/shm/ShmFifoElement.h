#pragma once
#include "ShmFifo.h"
#include "Element.h"
#include "ShmCfg.h"
namespace smf{
	class ShmFifoElement {
	public:
		ShmFifoElement(Element*this_,bool source);
	public:
		bool open();
		bool close();
	public:
		bool set(const char*);
		bool set(uint32_t key, void* val);
		bool get(uint32_t key, void* val) const;
		bool WaitMedia(uint32_t timeout=0);
		bool generateFrame(Frame* frm);
		bool receiveFrame(Frame* frm);
		operator bool() const { return _shm; }
		char* print(char*, char*)const;
	public:
		Element* _ele = 0;
		ShmFifo* _shm = 0;

		/////[notify] modify with shmcfg_t/////
		//shmcfg_t begin
		uint32_t _timeskip = 5;
		uint32_t _timeout = 200;
		//
		uint64_t _keys = 0;
		uint32_t _buff_max = 0;
		uint32_t _fifo_max = 0;
		uint32_t _sleep_ms = 0;
		//
		uint8_t _sharedIndex = 0;
		bool _isSource = false;
		bool _isCacheable = false;
		bool _noCopy = false;

		bool _isReader = false;
		bool _isRing = false;
		bool _noAlloc = false;
		bool _partread = false;
		bool _ignoreMedia = false;

		bool _noForward = false;
		bool _noBackward = false;
		uint8_t _align = 0;
		//uint8_t revss[1]; //align to 64
		//shmcfg_t end
	public:
		//shmcfg_t& Cfg()const { return *(shmcfg_t*)&this->_timeskip; }
		bool From(shmcfg_t&);
		bool To(shmcfg_t&)const;
	};
}
