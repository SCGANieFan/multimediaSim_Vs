#pragma once
#include"MTF.AudioMuxer.h"
class MTF_OpusMuxer :public MTF_AudioMuxer
{
public:
	MTF_OpusMuxer();
	~MTF_OpusMuxer();
protected:
	virtual mtf_i32 Init() final;
	virtual mtf_i32 receive(MTF_Data& iData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const char* _url = 0;
};

