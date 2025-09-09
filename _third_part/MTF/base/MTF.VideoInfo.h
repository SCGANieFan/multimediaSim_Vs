#pragma once
#include"MTF.Type.h"
class MTF_VideoInfo
{
public:
	MTF_VideoInfo();
	~MTF_VideoInfo();
public:
	mtf_i32 Set(const char* key, mtf_void* val);
	mtf_i32 Get(const char* key, mtf_void* val);
public:
	static mtf_i32 _rate;
	static mtf_i32 _frameMs;
	static mtf_i32 _frameSamples;
	static mtf_i32 _frameBytes;
	static mtf_i8 _ch;
	static mtf_i8 _width;
	static mtf_bool isInterlace;
};

