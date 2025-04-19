#if 1
#include "MAFA.WavDemux.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "WavDemux.h"

maf_void maf_wav_demux_register()
{
	MAF_Object::Registe<MAFA_WavDemux>("wav_demux");
}

maf_void* MAFA_WavDemux::_malloc = 0;
maf_void* MAFA_WavDemux::_free = 0;

MAFA_WavDemux::MAFA_WavDemux()
{
}
MAFA_WavDemux::~MAFA_WavDemux()
{
}

maf_int32 MAFA_WavDemux::Init()
{
	MAF_PRINT();
#if 1
	WavDemuxInitParam initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(WavDemuxInitParam));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();

	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting_t));
	AlgoBasePorting_t* basePorting = (AlgoBasePorting_t*)_basePorting;

	basePorting->Malloc = MallocLocal;
	basePorting->Free = FreeLocal;
	initParam.basePorting = basePorting;
#if 0
	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.width = _width;
	initParam.frameSamples = _frameSamples;
	initParam.overlapMs = _overlapMs;
	initParam.decayTimeMs = _decayMs;
#endif
#endif

	_hdSize = WavDemux_GetSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = WavDemux_Init(_hd, &initParam);

	if (ret < 0)
	{
		return -1;
	}
	return 0;
#endif
	return 0;
}

maf_int32 MAFA_WavDemux::Deinit()
{
	MAF_PRINT();
#if 1
	WavDemux_DeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);
#endif
	return 0;
}

maf_int32 MAFA_WavDemux::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	
	maf_int32 ret;
	maf_int32 outByte = dataOut->GetLeftSize();
	ret = WavDemux_Run(_hd, 
		dataIn->GetData(),
		dataIn->GetSize());
	if (ret < 0)
	{
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	dataOut->Append(outByte);
	return 0;
}

maf_int32 MAFA_WavDemux::Set(const maf_int8* key, maf_void* val)
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

maf_int32 MAFA_WavDemux::Get(const maf_int8* key, maf_void* val)
{
#if 1
	if (MAF_String::StrCompare(key, "hasHead")) {
		maf_uint32 hasHead;
		void* param[3] = { &hasHead };
		WavDemux_Get(_hd, WAV_DEMUX_GET_CHOOSE_HAS_HEAD, param);
		*(maf_uint32*)val = hasHead;
		if (hasHead > 0)
		{
			maf_int32 rate;
			maf_int32 ch;
			maf_int32 width;
			param[0] = &rate;
			param[1] = &ch;
			param[2] = &width;
			WavDemux_Get(_hd, WAV_DEMUX_GET_CHOOSE_BASIC_INFO, param);
			Set("rate", (void *)rate);
			Set("ch", (void*)ch);
			Set("width", (void*)width);
		}
		return 0;
	}
	else if (MAF_String::StrCompare(key, "dataPos")) {
		WavDemux_Get(_hd, WAV_DEMUX_GET_CHOOSE_DATA_POS, (void **)val); return 0;
	}
#endif
	return MAF_Audio::Get(key, val);
}

maf_void* MAFA_WavDemux::MallocLocal(int32_t size)
{
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = ((MAF_Memory::Malloc_t)_malloc)(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif	
}

maf_void MAFA_WavDemux::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((MAF_Memory::Free_t)_free)(block);
}

#endif