#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_OggDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_OggDemuxer();
	~MTF_OggDemuxer();

protected:
	virtual mtf_int32 Init() final;
	virtual mtf_int32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const mtf_int8* _url = 0;
private:
	MTF_Data _oData;
private:
	mtf_void* _hd = 0;
	mtf_int32 _hdSize = 0;
};

