#include "MTF.MusicPlc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"
#include "plc_api.h"

void mtf_music_plc_register()
{
	MTF_Objects::Registe<MTF_MusicPlc>("music_plc");
	//plc_api_register_music_plc_int16();
	//plc_api_register_music_plc_int32();
	//plc_api_register_music_plc_f32();
	//plc_api_register_sbc_plc_i16();
	plc_api_register_ts_plc_i16();
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
	if (_hd)
	{
		MTF_PRINT();
		if(_hd)
			plc_api_destory(_hd);
		_hd = 0;
	}
}

void* PlcMalloc(int size) {
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
	MTF_PRINTORI("%s", buf);
}

mtf_i32 MTF_MusicPlc::Init()
{	
	//lib init
#if 0
	ret = MAF_Init(_hd, script, param);
#else
	MTF_PRINT();
	plc_api_param_t initParam;
	MTF_MEM_SET(&initParam, 0, sizeof(plc_api_param_t));
	initParam.fs_hz = _rate;
	initParam.channels = _ch;
	initParam.frame_samples = _frameSamples;
	initParam.channel_select = 0xffff;
	initParam.data_type = plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16;
	//initParam.dataType = plc_api_data_type_e::PLC_API_DATA_TYPE_INT_32;
	//initParam.dataType = plc_api_data_type_e::PLC_API_DATA_TYPE_FLOAT_32;
#if 0
	initParam.mode = plc_api_mode_e::PLC_API_MODE_MUSIC_PLC;
#if 1
	initParam.param_set = plc_api_param_set_e::PLC_API_PARAM_SET_KEY_APP;
	initParam.key = PLC_API_KEY;
	initParam.application = plc_api_application_e::PLC_API_APPLICATION_AUTO;
	//initParam.application = plc_api_application_e::PLC_API_APPLICATION_MUSIC;
#else
	initParam.param_set = plc_api_param_set_e::PLC_API_PARAM_SET_PARAM;
	initParam.overlap_samples = 5 * initParam.fs_hz / 1000;
	initParam.hold_samples_after_lost = 0 * initParam.fs_hz / 1000;
	initParam.attenuate_samples_after_lost = 50 * initParam.fs_hz / 1000;
	initParam.gain_samples_after_no_lost = 50 * initParam.fs_hz / 1000;
	initParam.seek_samples = 15 * initParam.fs_hz / 1000;
	initParam.no_seek_samples = 4 * initParam.fs_hz / 1000;
	initParam.match_samples = 4 * initParam.fs_hz / 1000;
#endif
#endif
#if 0
	initParam.mode = plc_api_mode_e::PLC_API_MODE_SBC_PLC;
	initParam.param_set = plc_api_param_set_e::PLC_API_PARAM_SET_PARAM;
#if 0
	initParam.sbc_plc.codec_type = sbc_plc_codec_type_e::SBC_PLC_CODEC_TYPE_SBC;
#else
	initParam.sbc_plc.search_size = 256;
	initParam.sbc_plc.match_size = 64;
	initParam.sbc_plc.olal = 60;
	initParam.sbc_plc.recover = 60;
#endif
#endif
#if 1
	initParam.mode = plc_api_mode_e::PLC_API_MODE_TS_PLC;
	initParam.param_set = plc_api_param_set_e::PLC_API_PARAM_SET_PARAM;
	initParam.ts_plc.recv = 0;
#endif

	initParam.cb_malloc = PlcMalloc;
	initParam.cb_free = PlcFree;
	initParam.cb_printf = PlcPrint;
	//_hd = _memory.Malloc(_hdSize);
	plc_api_ret_t ret = plc_api_create(&_hd, &initParam);
	if (ret != PLC_API_RET_SUCCESS) {
		MTF_PRINT("plc_api_create fail, %d", ret);
		return -1;
	}

#endif

	//io data
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_u8*)MTF_MALLOC(2*size), 2 * size);

	return 0;
}

mtf_i32 MTF_MusicPlc::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
	return 0;
}

#define FRAMES_LOST 1
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
	mtf_i32 outByte = _oData.LeftSize();
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
	if (_iData._flags& MTF_DataFlag_EMPTY) {
		_iData._flags & ~MTF_DataFlag_EMPTY;
		ret = plc_api_run(
			_hd,
			NULL,
			0,
			0,
			_oData.LeftData(),
			&outByte,
			0xffff);
		_oData._size+= outByte;
	}
	else {
		ret = plc_api_run(
			_hd,
			_iData.Data(),
			_iData._size,
			0,
			_oData.LeftData(),
			&outByte,
			0x0000);
		_oData._size += outByte;
	}

	if (ret !=PLC_API_RET_SUCCESS)
	{
		return -1;
	}
	_iData.Used(_iData._size);
	_iData.Clear();
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
