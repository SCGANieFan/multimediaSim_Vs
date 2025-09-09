#pragma once
#include"MTF.Process.h"
#include"MTF.VideoInfo.h"

class MTF_VideoProcess :public MTF_Process, public MTF_VideoInfo
{
public:
	MTF_VideoProcess();
	virtual ~MTF_VideoProcess();
protected:
	virtual mtf_i32 Init() = 0;
	virtual mtf_i32 receive(MTF_Data& iData)=0;
	virtual mtf_i32 generate(MTF_Data*& oData)=0;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;
protected:
};

