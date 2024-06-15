#if 1
#include "MAFA.ApeDemux.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "ApeDemux.h"

maf_void maf_ape_demux_register()
{
	MAF_Object::Registe<MAFA_ApeDemux>("ape_demux");
}

maf_void* MAFA_ApeDemux::_malloc = 0;
maf_void* MAFA_ApeDemux::_free = 0;

MAFA_ApeDemux::MAFA_ApeDemux()
{
}
MAFA_ApeDemux::~MAFA_ApeDemux()
{
}

maf_int32 MAFA_ApeDemux::Init()
{
	MAF_PRINT();
	ApeDemuxInitParam_t initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(ApeDemuxInitParam_t));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();
	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting));
	AlgoBasePorting* basePorting = (AlgoBasePorting*)_basePorting;
	basePorting->Malloc = (ALGO_Malloc_t)MallocLocal;
	basePorting->Free = (ALGO_Free_t)FreeLocal;
	initParam.basePorting = basePorting;
#endif
	initParam.mode = ApeDemuxInitMode_e::APE_DEMUX_INIT_MODE_DMUX;

	_hdSize = ApeDemux_GetSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = ApeDemux_Init(_hd, &initParam);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

maf_int32 MAFA_ApeDemux::Deinit()
{
	MAF_PRINT();
	ApeDemux_DeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);
	return 0;
}

maf_int32 MAFA_ApeDemux::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	maf_int32 ret;
	ret = ApeDemux_Run(_hd, dataIn->GetData(), dataIn->GetSize());
	if (ret < 0) {
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	return 0;
}

maf_int32 MAFA_ApeDemux::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_ApeDemux::Get(const maf_int8* key, maf_void* val)
{

	if (MAF_String::StrCompare(key, "apeHeader")) {
		ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_HEADE, val); return 0;
	}
	else if (MAF_String::StrCompare(key, "startPosFromFrame")) {
		ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_START_POS_FROM_FRAME, val); return 0;
	}
	else if (MAF_String::StrCompare(key, "rate")) {
		ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_RATE, val); return 0;
	}
	else if (MAF_String::StrCompare(key, "ch")) {
		ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_CH, val); return 0;
	}
	else if (MAF_String::StrCompare(key, "width")) {
		ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_WIDTH, val); return 0;
	}
	return MAF_Audio::Get(key, val);
}

maf_void* MAFA_ApeDemux::MallocLocal(int32_t size)
{
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = ((ALGO_Malloc_t)_malloc)(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
}

maf_void MAFA_ApeDemux::FreeLocal(maf_void* block)
{
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
	return ((ALGO_Free_t)_free)(block);
}

#endif