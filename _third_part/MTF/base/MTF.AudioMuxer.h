#pragma once
#include"MTF.Sink.h"
#include"MTF.AudioInfo.h"
class MTF_AudioMuxer :public MTF_Sink, public MTF_AudioInfo
{
public:
	MTF_AudioMuxer();
	~MTF_AudioMuxer();
public:
	virtual mtf_i32 receive(MTF_Data& iData) override;
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;
private:
	
};

