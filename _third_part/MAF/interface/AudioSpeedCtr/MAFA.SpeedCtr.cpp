#if 1
#include "MAFA.SpeedCtr.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "AudioSpeedControl.h"

maf_void maf_auio_speedCtr_register()
{
	MAF_Object::Registe<MAFA_SpeedCtr>("auio_speedCtr");
}

maf_void* MAFA_SpeedCtr::_malloc = 0;
maf_void* MAFA_SpeedCtr::_free = 0;

MAFA_SpeedCtr::MAFA_SpeedCtr()
{
}
MAFA_SpeedCtr::~MAFA_SpeedCtr()
{
}

maf_int32 MAFA_SpeedCtr::Init()
{
	MAF_PRINT();
#if 1
	_malloc = _memory.GetMalloc();
	_free= _memory.GetFree();
#endif

#if 1
	_hdSize = AudioSpeedControl_GetStateSize();
	_hd = _memory.Malloc(_hdSize);
	if (!_hd) {
		return -1;
	}
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
#if 1
	AudioSpeedInitParam param;
	MAF_MEM_SET(&param, 0, sizeof(AudioSpeedInitParam));
	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting_t));
	AlgoBasePorting_t* basePorting = (AlgoBasePorting_t*)_basePorting;

	basePorting->Malloc = MallocLocal;
	basePorting->Free = FreeLocal;
	param.basePorting = basePorting;
	param.fsHz = _rate;
	param.width = _width;
	param.channels = _ch;
	param.seekMs = AUDIO_SPEED_REF_VAL_SEEK_MS;
	param.overlapMs = AUDIO_SPEED_REF_VAL_OVERLAP_MS;
	param.constMs = AUDIO_SPEED_REF_VAL_CONST_MS;
	param.speed = _speed;
#endif
	AudioSpeedControl_Init(_hd, &param);

#if 1
	//set
	maf_void* ascSetParam[] = {
		(maf_void*)(maf_uint32)((1.5f) * (1 << 8)),
	};
	AudioSpeedControl_Set(_hd, AudioSpeedControl_SetChhoose_e::AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8, ascSetParam);

	//get
	maf_int32 speedQ8;
	maf_void* ascGetParam[] = {
		(maf_void*)(maf_uint32)(&speedQ8),
	};
	AudioSpeedControl_Get(_hd, AudioSpeedControl_GetChhoose_e::AUDIO_SPEED_CONTROL_GET_CHOOSE_SPEEDQ8, ascGetParam);
#endif

	int32_t size;
	maf_void* buf;
	size = 10 * 1024;
	buf = _memory.Malloc(size);
	_oDataCache.Init((maf_uint8*)buf, 0, size);
	
#endif
	return 0;
}

maf_int32 MAFA_SpeedCtr::Deinit()
{
	MAF_PRINT();
#if 1
	MAF_PRINT("_hd=%x", (maf_uint32)_hd);
	AudioSpeedControl_DeInit(_hd);
	_memory.Free(_hd);
	if(_basePorting)
		_memory.Free(_basePorting);

#endif
	return 0;
}

maf_int32 MAFA_SpeedCtr::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	static int num = 0;
#if 1
	int32_t inUsed;
	int32_t outLen;
	
	outLen = _oDataCache.GetLeftSize();
	AudioSpeedControl_Run(
		_hd,
		dataIn->GetData(),
		dataIn->GetSize(),
		_oDataCache.GetLeftData(),
		&outLen);
	_oDataCache.Append(outLen);
	if (_oDataCache.GetSize() > 0)
	{
		maf_int32 appendSize = _oDataCache.GetSize();
		appendSize = appendSize > dataOut->GetLeftSize() ? dataOut->GetLeftSize() : appendSize;
		dataOut->Append(_oDataCache.GetData(), appendSize);
		_oDataCache.Used(appendSize);
		_oDataCache.ClearUsed();
	}

	MAF_PRINT("[%d],isize:%d,osize:%d", num++, dataIn->GetSize(), dataOut->GetSize());
	dataIn->Used(dataIn->GetSize());
#endif
	return 0;
}

maf_int32 MAFA_SpeedCtr::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "speedQ8")){
		_speed = (maf_float)(maf_uint32)val / (1 << 8); return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_SpeedCtr::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

#if 1
maf_void* MAFA_SpeedCtr::MallocLocal(int32_t size)
{
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
#if 0
	maf_void* ptr = ((ALGO_Malloc_t)_malloc)(size);
#else
	maf_void* ptr = malloc(size);
#endif
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif	
}

maf_void MAFA_SpeedCtr::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
#if 0
	return ((ALGO_Free_t)_free)(block);
#else
	return (free)(block);
#endif
}
#endif
#endif