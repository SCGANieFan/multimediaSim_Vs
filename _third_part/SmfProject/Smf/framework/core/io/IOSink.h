#pragma once
#include "Sink.h"
#include "IO.h"
#include "URL.h"
//#include "IOCacheWriter.h"
namespace smf {
	class IOSink
		: public Sink
	{
	public:
		IOSink();
		virtual ~IOSink();
	protected:
		virtual bool open(void*)override;
		virtual bool close()override;
		virtual bool receiveFrame(Frame*)override;
		virtual bool set(uint32_t key, void* val) override;
		virtual bool setParam(void*)override;
	protected:
		IO* _io = 0;
		URL _url;
		//IOCacheWriter _writer;
		//int _ioCacheLen = 1024;
		//int _ioCacheAlign = 64;
	};
}
