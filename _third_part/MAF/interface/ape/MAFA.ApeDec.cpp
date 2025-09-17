#if 1
#include "MAFA.ApeDec.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "ApeDec.h"

maf_void maf_ape_dec_register()
{
	MAF_Object::Registe<MAFA_ApeDec>("ape_dec");
}

maf_void* MAFA_ApeDec::_malloc = 0;
maf_void* MAFA_ApeDec::_free = 0;

maf_int32 MAFA_ApeDec::Init()
{
	MAF_PRINT();
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();
	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting_c));
	AlgoBasePorting_c* basePorting = (AlgoBasePorting_c*)_basePorting;
	//basePorting->Malloc = (ALGO_Malloc_t)MallocLocal;
	//basePorting->Free = (ALGO_Free_t)FreeLocal;
	return 0;
}

maf_int32 MAFA_ApeDec::Deinit()
{
	MAF_PRINT();
	ApeDec_DeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);

	return 0;
}

inline maf_bool MAFA_ApeDec::HdInit(MAF_Data* dataIn) {
	_hdSize = ApeDec_GetSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd) {
		return -1;
	}

	ApeDecInitParam_t initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(ApeDecInitParam_t));
	initParam.basePorting = (AlgoBasePorting*)_basePorting;
	initParam.context = (void*)*(maf_uint32*)dataIn->GetData();
	initParam.startFrame = *(maf_uint32*)(dataIn->GetData() + 4);
	initParam.skip = *(maf_uint32*)(dataIn->GetData() + 8);
	maf_int32 ret = ApeDec_Init(_hd, &initParam);
	dataIn->Used(12);
	if (ret < 0) {
		return false;
	}
	return true;
}

maf_int32 MAFA_ApeDec::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{

	if (dataIn->CheckFlag(MAFA_FRAME_IS_EXTRA_INFO)){
		dataIn->ClearFlag(MAFA_FRAME_IS_EXTRA_INFO);
		if (!HdInit(dataIn))
			return false;
	}

	if (dataIn->CheckFlag(MAFA_FRAME_IS_EOS)){
		ApeDec_Set(_hd, ApeDecSet_e::APE_DEC_SET_E_HAS_IN_CACHE, (void*)false);
	}

	maf_int32 outByte = dataOut->GetLeftSize();
	maf_int32 ret = ApeDec_Run(_hd, dataIn->GetData(), dataIn->GetSize(), dataOut->GetLeftData(), &outByte);
	if (ret < 0) {
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	dataOut->Append(outByte);
	return 0;
}

maf_int32 MAFA_ApeDec::Set(const maf_int8* key, maf_void* val)
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

maf_int32 MAFA_ApeDec::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

maf_void* MAFA_ApeDec::MallocLocal(int32_t size)
{
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = ((ALGO_Malloc_t)_malloc)(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif	
}

maf_void MAFA_ApeDec::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((ALGO_Free_t)_free)(block);
}

#endif