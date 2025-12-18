#include <stdio.h>
#include "MTF.MusicPlc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"
#include "plc_api.h"
#include <stdio.h>

#define	SIM_PLC_RUN				1
#define	SIM_PLC_R_G				(1&&(!SIM_PLC_RUN))
#define	SIM_PLC_FILL_ZERO		(1&&(!SIM_PLC_RUN)&&(!SIM_PLC_R_G))

void mtf_music_plc_register()
{
	MTF_Objects::Registe<MTF_MusicPlc>("music_plc");
	plc_api_register_music_plc_int16();
	plc_api_register_music_plc_int32();
	//plc_api_register_music_plc_f32();
	//plc_api_register_sbc_plc_i16();
	//plc_api_register_ts_plc_i16();
}
MTF_MusicPlc::MTF_MusicPlc()
{
}

MTF_MusicPlc::~MTF_MusicPlc()
{
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_plcApiId)
	{
		MTF_PRINT();
		if (_plcApiId) {
			plc_api_close(_plcApiId);
			plc_api_destory(_plcApiId);
		}
		_plcApiId = 0;
	}
}

void* PlcMalloc(uint32_t size) {
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

static void PlcFree(void* ptr) {
#if 1
	MTF_PRINT("free, ptr:%x", (mtf_u32)ptr);
#endif
	return Free(ptr);
}

static void PlcPrint(const char* fmt, ...) {
	char buf[256];
	VaListPorting_t args;
	VaStartPorting(args, fmt);
	VsprintfPorting(buf, fmt, args);
	VaEndPorting(args);
	//MTF_PRINTORI("%s", buf);
	//PrintfOri("%s", buf);
	printf("%s", buf);
}

mtf_i32 MTF_MusicPlc::Init()
{	
	//lib init
	MTF_PRINT();
	plc_api_param_t initParam;
	MTF_MEM_SET(&initParam, 0, sizeof(plc_api_param_t));
	initParam.malloc_cb = PlcMalloc;
	initParam.free_cb = PlcFree;
	initParam.print_cb = PlcPrint;
	_plcApiId = plc_api_create(&initParam);
	if (!_plcApiId) {
		MTF_PRINT("plc_api_create fail");
		return -1;
	}

	plc_api_set(_plcApiId, "rate", (void*)(uint32_t)_rate);
	plc_api_set(_plcApiId, "ch", (void*)(uint32_t)_ch);
	plc_api_set(_plcApiId, "fSample", (void*)(uint32_t)_frameSamples);
	plc_api_set(_plcApiId, "chSelect", (void*)(uint32_t)0xffff);
	plc_api_set(_plcApiId, "dataType", (void*)"i16");//f32,i32,i16
	plc_api_set(_plcApiId, "mode", (void*)"musicPlc");
	plc_api_set(_plcApiId, "paramSet", (void*)"param");//keyapp,param
	plc_api_set(_plcApiId, "overlap", (void*)(uint32_t)(1 * _rate / 1000));
	plc_api_set(_plcApiId, "holdAL", (void*)(uint32_t)(0 * _rate / 1000));
	plc_api_set(_plcApiId, "attAL", (void*)(uint32_t)(20 * _rate / 1000));
	plc_api_set(_plcApiId, "gainANL", (void*)(uint32_t)(20 * _rate / 1000));
#if 1
	plc_api_set(_plcApiId, "forceMT", (void*)1);
#endif
#if 0
	plc_api_set(_plcApiId, "seek", (void*)(uint32_t)(15 * _rate / 1000));
	plc_api_set(_plcApiId, "noSeek", (void*)(uint32_t)(4 * _rate / 1000));
	plc_api_set(_plcApiId, "match", (void*)(uint32_t)(4 * _rate / 1000));
#else
	plc_api_set(_plcApiId, "seek", (void*)(uint32_t)(0 * _rate / 1000));
	plc_api_set(_plcApiId, "noSeek", (void*)(uint32_t)(10 * _rate / 1000));
	plc_api_set(_plcApiId, "match", (void*)(uint32_t)(0 * _rate / 1000));
#endif
	plc_api_open(_plcApiId);

	//io data
#if SIM_PLC_R_G
	mtf_i32 iSize = 0;
	plc_api_get(_plcApiId, "recByte", (void*)&iSize);
	_iData.Init((mtf_u8*)MTF_MALLOC(iSize), iSize);
	MTF_MEM_SET(_iData.Data(), 0, _iData._size);

	mtf_i32 overlapByte = 0;
	plc_api_get(_plcApiId, "ovByte", (void*)&overlapByte);
	_iData._size += overlapByte;
#else
	mtf_i32 iSize = 40 * _width * _ch * _rate / 1000;
	_iData.Init((mtf_u8*)MTF_MALLOC(iSize), iSize);
#endif
	mtf_i32 oSize = 2 * _frameBytes;
	_oData.Init((mtf_u8*)MTF_MALLOC(oSize), oSize);


	return 0;
}

mtf_i32 MTF_MusicPlc::receive(MTF_Data& iData)
{
	if (_iData.LeftSize() < iData._size) {
		_iData.Clear(iData._size - _iData.LeftSize());
	}
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
	return 0;
}

#define FRAMES_LOST 5
#define FRAMES_TOTAL 100
mtf_i32 MTF_MusicPlc::generate(MTF_Data*& oData)
{
	//_frames++;
	if ((_frames % FRAMES_TOTAL) + 1 > (FRAMES_TOTAL - FRAMES_LOST))
		_iData._flags |= MTF_DataFlag_EMPTY;
	else
		_iData._flags &= ~MTF_DataFlag_EMPTY;

	_frames++;

	mtf_i32 ret = 0;
	mtf_i32 inByte = _iData._size;
	mtf_i32 outByte = _oData.LeftSize();
#if SIM_PLC_FILL_ZERO
	if (_iData._flags & MTF_DataFlag_EMPTY) {
		outByte = _frameSamples * _width * _ch;
		MTF_MEM_SET(_oData.LeftData(), 0, outByte);
		_oData._size += outByte;
	}
	else {
		_oData.Append(_iData.Data(), _iData._size);
	}
	_iData.Used(_iData._size);
	_iData.Clear();
#endif

#if SIM_PLC_RUN
	if (_iData._flags& MTF_DataFlag_EMPTY) {
		_iData._flags & ~MTF_DataFlag_EMPTY;
		ret = plc_api_run(
			_plcApiId,
			NULL,
			0,
			_oData.LeftData(),
			&outByte,
			0xffff);
		_oData._size+= outByte;
	}
	else {
		ret = plc_api_run(
			_plcApiId,
			_iData.Data(),
			&inByte,
			_oData.LeftData(),
			&outByte,
			0x0000);
		_oData._size += outByte;
	}

	if (ret !=PLC_API_RET_SUCCESS)
	{
		return -1;
	}
#if 0
	uint16_t* pIn = (uint16_t*)_iData.Data();
	uint16_t* pOut = (uint16_t*)_oData.Data();
	if (_ch == 2) {
		for (uint32_t f = 0; f < _frameSamples; f++) {
			pOut[f * _ch] = pIn[f * _ch];
		}
	}
	
#endif
	_iData.Used(_iData._size);
	_iData.Clear();
#endif
#if SIM_PLC_R_G
	static int32_t _lost_num = 0;
	if (_iData._flags & MTF_DataFlag_EMPTY) {
		_iData._flags & ~MTF_DataFlag_EMPTY;
		_lost_num++;
		if (_lost_num==1) {
			plc_api_receive(_plcApiId, _iData.Buff(), _iData.Max() - _iData.LeftSize());
		}
		int32_t oByte = _frameBytes;
		plc_api_generate(_plcApiId, _oData.LeftData(), &oByte);
		_oData._size += oByte;
	}
	else {
		_oData.Append(_iData.Data(), _frameBytes);
		_lost_num = 0;
	}
	_iData.Used(_frameBytes);
#endif
	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
	oData = &_oData;
	// MTF_PRINT("[%d]%d", _frames,oData->_size);
	return 0;
}

mtf_i32 MTF_MusicPlc::Set(const char* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "gainMs")) {
		_gainMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_i16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_MusicPlc::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
