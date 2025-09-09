#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_OggDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_OggDemuxer();
	~MTF_OggDemuxer();

protected:
	virtual mtf_i32 Init() final;
	virtual mtf_i32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const char* _url = 0;
private:
	MTF_Data _oData;
private:
	mtf_void* _hd = 0;
	mtf_i32 _hdSize = 0;
};

