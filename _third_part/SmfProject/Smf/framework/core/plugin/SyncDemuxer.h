#pragma once
#include "IOSource.h"
namespace smf{
namespace audio {
	class SyncDemuxer 
		:public IOSource
	{
	public:
		SyncDemuxer();
		virtual ~SyncDemuxer();
	protected:
		virtual bool open(void* param)override;
		//virtual bool close()override;
		virtual bool generateFrame(Frame*&)override;
		virtual bool seekToIndex(uint32_t frameindex) override;
		virtual bool set(uint32_t key, void* val) override;
	protected:
		virtual bool Parse();
	protected:
		int syncFrame(int rcnt, uint8_t*frame = 0);
		int syncFrame(uint8_t*& ptr, uint8_t* end);
		bool headerParse();
	private:
		int _syncLeft = 0;
		uint8_t* _frame = 0;
		uint8_t _buffsync[32];
		uint8_t _frameHeader[16];
		uint8_t _checkFrameCount = 3;
		bool _parsed = false;
		bool _scan = false;
	protected:
		int _fsize = 0;
		int _hsize = 0;
		int _hend = 0;
		int _frameHeadLen = 0;
		int _fcnt = 0;
		int _fdura = 0;//us
	protected:
		virtual bool parseFileHeader() = 0;
		virtual int checkFrameHeader(void* ptr) = 0;
		virtual bool parseFrameHeader(void* ptr) = 0;
		virtual bool compareFrameHeader(void* header0, void* header1) = 0;
	};
}
}
