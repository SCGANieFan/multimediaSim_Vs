#pragma once
#include"MTF.AudioMuxer.h"
class MTF_OpusMuxer :public MTF_AudioMuxer
{
public:
	MTF_OpusMuxer();
	~MTF_OpusMuxer();
protected:
	virtual mtf_int32 Init() final;
	virtual mtf_int32 receive(MTF_Data& iData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const mtf_int8* _url = 0;
};

