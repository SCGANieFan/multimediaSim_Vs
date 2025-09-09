#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_PcmDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_PcmDemuxer();
	~MTF_PcmDemuxer();

private:
	virtual mtf_i32 Init() final;

public:
	virtual mtf_i32 generate(MTF_Data*& oData) final;
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const char* _url = 0;
private:
	MTF_Data _oData;
};

