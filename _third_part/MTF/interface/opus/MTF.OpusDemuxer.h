#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_OpusDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_OpusDemuxer();
	~MTF_OpusDemuxer();
protected:
	virtual mtf_int32 Init() final;
	virtual mtf_int32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
	MTF_Data _oData;
	void* _pFile = 0;
	const mtf_int8* _url = 0;
};

