#pragma once
#include"MTF.Process.h"
#include"MTF.AudioInfo.h"

class MTF_AudioProcess :public MTF_Process, public MTF_AudioInfo
{
public:
	MTF_AudioProcess();
	virtual ~MTF_AudioProcess();
protected:
	virtual mtf_i32 Init() = 0;
	virtual mtf_i32 receive(MTF_Data& iData) = 0;
	virtual mtf_i32 generate(MTF_Data*& oData) = 0;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;
protected:
};

