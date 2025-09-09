#pragma once
#include"MTF.Sink.h"
class MTF_IOSink :public MTF_Sink
{
public:
	MTF_IOSink();
	~MTF_IOSink();
private:
	virtual mtf_i32 Init() final;
public:
	virtual mtf_i32 receive(MTF_Data& iData) final;
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;
private:
	MTF_Data _iData;
private:
	void* _pFile = 0;
	const char* _url = 0;
	mtf_i32 _iPortMax = 1*1024;
	mtf_i32 _iPortMin = 1*1024;
	
};

