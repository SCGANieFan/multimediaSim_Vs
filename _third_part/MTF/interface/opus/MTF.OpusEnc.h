#pragma once
#include"MTF.AudioProcess.h"
class MTF_OpusEnc :public MTF_AudioProcess
{
public:
	MTF_OpusEnc();
	~MTF_OpusEnc();
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

	mtf_i32 _bitrate = 12000;
	mtf_i32 _complexity = 0;
	mtf_bool _vbr = false;
	mtf_i32 _pcmFrameSample = 0;
	mtf_i32 _pcmFrameByte = 0;
	void* _enc = 0;
};

