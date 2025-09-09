#pragma once
#include"MTF.AudioMuxer.h"
class MTF_PcmMuxer :public MTF_AudioMuxer
{
public:
	MTF_PcmMuxer();
	~MTF_PcmMuxer();
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
	
};

