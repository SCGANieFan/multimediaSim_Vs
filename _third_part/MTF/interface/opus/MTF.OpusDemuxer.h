#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_OpusDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_OpusDemuxer();
	~MTF_OpusDemuxer();
protected:
	virtual mtf_i32 Init() final;
	virtual mtf_i32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	MTF_Data _oData;
	void* _pFile = 0;
	const char* _url = 0;
};

