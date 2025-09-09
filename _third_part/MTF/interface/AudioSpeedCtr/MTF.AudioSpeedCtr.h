#pragma once
#include"MTF.AudioProcess.h"
class MTF_AudioSpeedCtr :public MTF_AudioProcess
{
public:
	MTF_AudioSpeedCtr();
	~MTF_AudioSpeedCtr();
protected:
	virtual mtf_i32 Init() final;
	virtual mtf_i32 receive(MTF_Data& iData) final;
	virtual mtf_i32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	MTF_Data _iData;
	MTF_Data _oData;

	mtf_void* _hd = 0;
	mtf_i32 _hdSize = 0;

	mtf_i32 _speedQ8 = 0;

	mtf_i32 _frames = 0;

};

