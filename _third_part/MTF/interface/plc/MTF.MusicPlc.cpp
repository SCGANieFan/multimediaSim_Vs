#include "MTF.MusicPlc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"
#include "plc_api.h"

void mtf_music_plc_register()
{
	MTF_Objects::Registe<MTF_MusicPlc>("music_plc");
	plc_api_register_music_plc_int16();
	//plc_api_register_music_plc_int32();
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
	initParam.mode = plc_api_mode_e::PLC_API_MODE_MUSIC_PLC;
#if 0
	initParam.param_set = plc_api_param_set_e::PLC_API_PARAM_SET_KEY_APP;
	initParam.key = PLC_API_KEY;
	initParam.application = plc_api_application_e::PLC_API_APPLICATION_AUTO;
	//initParam.application = plc_api_application_e::PLC_API_APPLICATION_MUSIC;
#else
	initParam.param_set = plc_api_param_set_e::PLC_API_PARAM_SET_PARAM;
	initParam.music_plc.overlap_samples = 1 * initParam.fs_hz / 1000;
	//initParam.music_plc.overlap_samples = 24;
	initParam.music_plc.hold_samples_after_lost = 0 * initParam.fs_hz / 1000;
	initParam.music_plc.attenuate_samples_after_lost = 20 * initParam.fs_hz / 1000;
	initParam.music_plc.gain_samples_after_no_lost = 20 * initParam.fs_hz / 1000;
	initParam.music_plc.seek_samples = 15 * initParam.fs_hz / 1000;
	initParam.music_plc.no_seek_samples = 4 * initParam.fs_hz / 1000;
	initParam.music_plc.match_samples = 4 * initParam.fs_hz / 1000;
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

#define FRAMES_LOST 3
#define FRAMES_TOTAL 200
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
