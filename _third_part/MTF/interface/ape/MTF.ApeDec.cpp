#include "MTF.ApeDec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "ApeDec.h"
#include "Ape.Inner.decoder.h"
using namespace mtf_ns;

void mtf_ape_dec_register()
{
	MTF_Objects::Registe<MTF_ApeDec>("ape_dec");
}

static mtf_void* MallocLocal(int32_t size)
{
#if 1
	static mtf_i32 sizeTotal = 0;
	sizeTotal += size;
	mtf_void* ptr = Malloc(size);
	MTF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (mtf_u32)ptr, size, sizeTotal);
	return ptr;
#else
	return Malloc(size);
#endif	
}

mtf_void FreeLocal(mtf_void* block)
{
#if 1
	MTF_PRINT("free, ptr:%x", (mtf_u32)block);
#endif
	return Free(block);
}

class ApeBasePorting_c :public AlgoBasePorting_c {
public:
	ApeBasePorting_c() {}
	~ApeBasePorting_c() {}
public:
	virtual void* Malloc(int32_t size) { 
		return malloc(size);
		//return mtf_ns::Malloc(size); 
	}
	virtual void Free(void* block) { 
		free(block);
		//mtf_ns::Free(block); 
	}
public:
	void (*print_cb)(const char* fmt, ...);
};
static ApeBasePorting_c apeBasePorting;
MTF_ApeDec::MTF_ApeDec()
{

}

MTF_ApeDec::~MTF_ApeDec()
{
#if 0
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		apeBasePorting.Free(_iData.Data());
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		apeBasePorting.Free(_oData.Data());
	}
	if (_hd)
	{
#if 0
		MAF_Deinit(_hd);
		MTF_FREE(_hd);
#else
		MTF_PRINT();
		ApeDec_DeInit(_hd);
		Free(_hd);
		Free(_basePorting);
#endif
	}
#endif
}

mtf_i32 MTF_ApeDec::Init()
{
	//lib init
#if 0
	ret = MAF_Init(_hd, script, param);
#else
	MTF_PRINT();
	//mtf_void PrintfOri(const char* format, ...);
	//mtf_void Printf(mtf_u16 ch, const char* format, ...);
	//mtf_void PrintfNoformat(const char* buf);
	apeBasePorting.print_cb = PrintfOri;
	_basePorting = &apeBasePorting;

#endif


	//io data
	mtf_i32 size = _frameBytes;
	
	_iData.Init((mtf_u8*)apeBasePorting.Malloc(size), size);
	size = 10 * size;
	_oData.Init((mtf_u8*)apeBasePorting.Malloc(size), size);

	return 0;
}

mtf_i32 MTF_ApeDec::receive(MTF_Data& iData)
{
	static uint32_t cnt = 0;
	cnt++;
	//MTF_PRINT("[%d]", cnt);
	_iData.Clear();
	mtf_i32 appenSize = iData._size;
	appenSize = appenSize > _iData.LeftSize() ? _iData.LeftSize() : appenSize;
	_iData.Append(iData.Data(), appenSize);
	iData.Used(appenSize);
	if (iData._flags & MTF_DataFlag_ESO)
	{
		_iData._flags = MTF_DataFlag_ESO;
	}
	if (iData._flags & MTF_DataFlag_EXTRA_INFO)
	{
		iData._flags &= ~MTF_DataFlag_EXTRA_INFO;
		_iData._flags = MTF_DataFlag_EXTRA_INFO;
	}
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_ApeDec::generate(MTF_Data*& oData)
{
	if (!oData) oData = &_oData;
	_frames++;
	//MTF_PRINT("[%d]", _frames);
	if (_iData._flags&MTF_DataFlag_EXTRA_INFO) {
#if 0
		_iData._flags&=~MTF_DataFlag_EXTRA_INFO;
		_hdSize = ApeDec_GetSize();
		_hd = Malloc(_hdSize);
		MTF_PRINT("_hd=%x,size:%d", (mtf_u32)_hd, _hdSize);
		if (!_hd) {
			return false;
		}
		ApeDecInitParam_t initParam;
		MTF_MEM_SET(&initParam, 0, sizeof(ApeDecInitParam_t));
		initParam.basePorting = (AlgoBasePorting_c*)_basePorting;
		initParam.context = (void*)*(mtf_u32*)_iData.Data();
		initParam.startFrame = *(mtf_u32*)(_iData.Data() + 4);
		initParam.skip = *(mtf_u32*)(_iData.Data() + 8);
		mtf_i32 ret = ApeDec_Init(_hd, &initParam);
		_iData.Used(12);
		if (ret < 0) {
			return false;
		}
#else
		_iData._flags &= ~MTF_DataFlag_EXTRA_INFO;
		_hd = new ApeDecoder_c();
		_pContext = (void*)*(mtf_u32*)_iData.Data();
		_startFrameNum = *(mtf_u32*)(_iData.Data() + 4);
		_skip = *(mtf_u32*)(_iData.Data() + 8);
		//_decReInit = (u32*)ptr; ptr += 4;
		//*_decReInit = 0;
		mtf_i32 ret = ((ApeDecoder_c*)_hd)->Init(
			(AlgoBasePorting_c*)_basePorting,
			(ApeContext_t*)_pContext,
			_startFrameNum, 
			_skip);
		_iData.Used(12);
		if (ret < 0) {
			return false;
		}
#endif
	}

#if 1
	if (_iData._flags & MTF_DataFlag_ESO) {
		((ApeDecoder_c*)_hd)->haveInCache = false;
	}
	uint8_t* inBuffer = (uint8_t*)_iData.Data();
	int32_t inSize = _iData._size;
	uint8_t* outBuffer = oData->LeftData();
	int32_t outSize = oData->LeftSize();
	APE_RET_t ret = ((ApeDecoder_c*)_hd)->Run(inBuffer, &inSize, outBuffer, &outSize);
	if (ret != APERET_SUCCESS) {
		return -1;
	}
	_iData.Used(inSize);
	_oData._size += outSize;
#else

	if (_iData._flags&MTF_DataFlag_ESO) {
		ApeDec_Set(_hd, ApeDecSet_e::APE_DEC_SET_E_HAS_IN_CACHE, (void*)false);
	}
	
	mtf_i32 outByte = oData->LeftSize();
	mtf_i32 iSize = _iData._size;
	mtf_i32 ret = ApeDec_Run(_hd, _iData.Data(), &iSize, oData->LeftData(), &outByte);
	if (ret < 0) {
		return -1;
	}

	_iData.Used(iSize);
	_oData._size+= outByte;
#endif
	//exit check
	if ((_iData._flags & MTF_DataFlag_ESO)
		&& _oData._size <= 0)
	{
		return -1;
	}
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_ApeDec::Set(const char* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_i16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_ApeDec::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
