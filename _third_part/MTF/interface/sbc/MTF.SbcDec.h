#pragma once
#include"MTF.AudioProcess.h"
class MTF_SbcDec :public MTF_AudioProcess
{
public:
	MTF_SbcDec();
	~MTF_SbcDec();
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

	mtf_i16 _decayMs = 0;
	mtf_i16 _gainMs = 0;
	mtf_i16 _overlapMs = 0;

	mtf_i32 _frames = 0;

};

