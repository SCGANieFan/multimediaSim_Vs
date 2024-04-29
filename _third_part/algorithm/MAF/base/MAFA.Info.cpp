#pragma once
#include"MAFA.Info.h"
#include"MAF.String.h"



MAFA_Info::MAFA_Info()
{
}
MAFA_Info::~MAFA_Info()
{
}


maf_int32 MAFA_Info::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "rate")) {
		_rate = (maf_int32)val;
		_frameSamples = _rate * _frameMs / 1000;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "fMs")) {
		_frameMs = (maf_int32)val;
		_frameSamples = _rate * _frameMs / 1000;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "fSamples")) {
		_frameSamples = (maf_int32)val;
		_frameMs = _frameSamples * 1000 / _rate;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "ch")) {
		_ch = (maf_int8)val; 
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "width")) {
		_width = (maf_int8)val;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "isInterlace")) {
		isInterlace = (maf_bool)val; return 0;
	}
	return -1;
}

maf_int32 MAFA_Info::Get(const maf_int8* key, maf_void* val)
{
#if 0
	return MTF_Process::Get(key, val);
#else
	return -1;
#endif
}