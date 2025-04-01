#if 1
#include <stdarg.h>
#include <stdio.h>
#include "MAFA.MusicPlc.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "PlcApi.h"
maf_void maf_music_plc_register()
{
	MAF_Object::Registe<MAFA_MusicPlc>("music_plc");
}

maf_void* MAFA_MusicPlc::_malloc = 0;
maf_void* MAFA_MusicPlc::_free = 0;

MAFA_MusicPlc::MAFA_MusicPlc()
{
}

MAFA_MusicPlc::~MAFA_MusicPlc()
{
}

static MAF_Memory *_PlcMemory = 0;
void* PlcMalloc(int size) {
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = _PlcMemory->Malloc(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif
}

static void PlcFree(void* ptr) {
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)ptr);
#endif
	return _PlcMemory->Free(ptr);
}

static void PlcPrint(const char* fmt, ...) {
	static maf_int8 buf[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	printf(buf);
}

maf_int32 MAFA_MusicPlc::Init()
{
	MAF_PRINT();
	PlcApiParam_t initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(PlcApiParam_t));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();
	_PlcMemory = &_memory;
#endif
	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.frameSamples = _frameSamples;
	initParam.width = _width;
	initParam.channelSelect = 0xffff;
	initParam.mode = PlcApiMode_e::PLC_API_MODE_MUSIC_PLC;
	//initParam.dataType = PlcApiDataType_e::PLC_API_DATA_TYPE_SHORT_16;
	initParam.dataType = PlcApiDataType_e::PLC_API_DATA_TYPE_INT_32;
	//initParam.dataType = PlcApiDataType_e::PLC_API_DATA_TYPE_FLOAT_32;
	initParam.application = PlcApiApplication_e::PLC_API_APPLICATION_MUSIC;
	initParam.cb_malloc = PlcMalloc;
	initParam.cb_free = PlcFree;
	initParam.cb_printf = PlcPrint;
	_hd = _memory.Malloc(_hdSize);
	PlcApiRet ret = PlcApiCreate(&_hd, &initParam);
	if (ret!=PLC_API_RET_SUCCESS){
		MAF_PRINT("PlcApiCreate fail, %d", ret);
		return -1;
	}

	return 0;
}

maf_int32 MAFA_MusicPlc::Deinit()
{
	MAF_PRINT();
	PlcApiDestory(_hd);
	_hd = 0;
	return 0;
}

maf_int32 MAFA_MusicPlc::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	maf_int32 ret = 0;

	maf_int32 outByte = dataOut->GetLeftSize();
#if 0
	if (dataIn->CheckFlag(MAFA_FRAME_IS_EMPTY)) {
		outByte = _frameSamples * _width * _ch;
		MAF_MEM_SET(dataOut->GetLeftData(), 0, outByte);
		dataOut->Append(outByte);
	}
	else {
		dataOut->Append(dataIn->GetData(), dataIn->GetSize());
	}

#else
	if (dataIn->CheckFlag(MAFA_FRAME_IS_EMPTY)) {
		dataIn->ClearFlag(MAFA_FRAME_IS_EMPTY);
		ret = PlcApiRun(
			_hd,
			NULL,
			0,
			0,
			dataOut->GetLeftData(),
			&outByte,
			0xffff);
		dataOut->Append(outByte);
	}
	else {
		dataOut->Append(dataIn->GetData(), dataIn->GetSize());
		ret = PlcApiRun(
			_hd,
			dataOut->GetData(),
			dataOut->GetSize(),
			0,
			dataOut->GetData(),
			&outByte,
			0x0000);
	}
#endif

	if (ret < 0)
	{
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	return 0;
}

maf_int32 MAFA_MusicPlc::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "gainMs")) {
		_gainMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_MusicPlc::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

#endif