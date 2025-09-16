#pragma once
#include"MTF.AudioMuxer.h"
class MTF_WavMuxer :public MTF_AudioMuxer
{
public:
	MTF_WavMuxer();
	~MTF_WavMuxer();
protected:
	virtual mtf_i32 Init() final;
	virtual mtf_i32 receive(MTF_Data& iData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;
private:
	void* _pFile = 0;
	const char* _url = 0;
private:
	MTF_Data _oData;
	MTF_Data _head;
private:
	mtf_void* _hd = 0;
	mtf_i32 _hdSize = 0;
	mtf_void* _basePorting;

};

