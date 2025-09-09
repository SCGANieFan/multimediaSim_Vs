#pragma once
#include"MTF.Source.h"
#include"MTF.AudioInfo.h"
class MTF_AudioDemuxer :public MTF_Source, public MTF_AudioInfo
{
public:
	MTF_AudioDemuxer();
	~MTF_AudioDemuxer();
public:
	virtual mtf_i32 generate(MTF_Data*& oData) override;
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;
private:
};

