#pragma once
// #include"MTF.AudioDemuxer.h"
#include "MTF.Source.h"
class MTF_IOSource :public MTF_Source
{
public:
	MTF_IOSource();
	~MTF_IOSource();

private:
	virtual mtf_i32 Init() final;

public:
	virtual mtf_i32 generate(MTF_Data*& oData) final;
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const char* _url = 0;
	mtf_i32 _oPortMax = 1*1024;
	mtf_i32 _oPortMin = 1*1024;
private:
	MTF_Data _oData;
};

