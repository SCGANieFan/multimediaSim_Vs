#if 1
#include "MAFA.WavMux.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "WavMux.h"

maf_void maf_wav_mux_register()
{
	MAF_Object::Registe<MAFA_WavMmux>("wav_mux");
}

maf_void* MAFA_WavMmux::_malloc = 0;
maf_void* MAFA_WavMmux::_free = 0;

MAFA_WavMmux::MAFA_WavMmux()
{
}
MAFA_WavMmux::~MAFA_WavMmux()
{
}

maf_int32 MAFA_WavMmux::Init()
{
	MAF_PRINT();
#if 1
	WavMuxInitParam initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(WavMuxInitParam));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();

	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting_t));
	AlgoBasePorting_t* basePorting = (AlgoBasePorting_t*)_basePorting;

	basePorting->Malloc = MallocLocal;
	basePorting->Free = FreeLocal;
	initParam.basePorting = basePorting;
#endif
#if 0
	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.width = _width;
#endif
	_hdSize = WavMux_GetSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = WavMux_Init(_hd, &initParam);

	if (ret < 0)
	{
		return -1;
	}
	return 0;
#endif
	return 0;
}

maf_int32 MAFA_WavMmux::Deinit()
{
	MAF_PRINT();
#if 1
	WavMux_DeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);
#endif
	return 0;
}

maf_int32 MAFA_WavMmux::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
#if 1
	maf_int32 ret;
	ret = WavMux_Run(_hd,
		dataIn->GetData(),
		dataIn->GetSize(),
		0, 0);
		
	if (ret < 0)
	{
		return -1;
	}
#if 0
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	dataOut->Append(outByte);
#endif
	return 0;
#endif
	return 0;
}

maf_int32 MAFA_WavMmux::Set(const maf_int8* key, maf_void* val)
{
#if 1
	if (MAF_String::StrCompare(key, "basicInfo")) {
		void* param[] = {(void*)_rate,(void*)_ch,(void*)_width};
		WavMux_Set(_hd, WAV_MUX_SET_CHOOSE_BASIC_INFO, param); return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_WavMmux::Get(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "headSize")) {
		WavMux_Get(_hd, WAV_MUX_GET_CHOOSE_HEAD_SIZE, val); return 0;
	}
	else if (MAF_String::StrCompare(key, "headInfo")) {
		WavMux_Get(_hd, WAV_MUX_GET_CHOOSE_HEAD, val); return 0;
	}
	return MAF_Audio::Get(key, val);
}

maf_void* MAFA_WavMmux::MallocLocal(int32_t size)
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

maf_void MAFA_WavMmux::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((MAF_Memory::Free_t)_free)(block);
}

#endif