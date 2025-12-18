#pragma once
#include "plc_api.h"
#include "gaapi.h"
#include "gaapi_gaf.h"
#include "music_plc.h"
using namespace plc_gaapi_ns;

#define LOG_PLC(fmt,...)		   LOG_GAAPI(fmt,##__VA_ARGS__)

struct plcApiParam_t {
	int32_t fs_hz;
	int16_t channels;
	int16_t frame_samples;
	const char* data_type;
	const char* mode;
	union {
		musicplc_param_t musicPlcParam;
	};
	//commonParam
	void* other;
};


class plc_api_c :public Gaapi_c{
public:
	plc_api_c() {}
	virtual ~plc_api_c() {}
public:
	virtual plc_api_ret_t Open()override;
	virtual plc_api_ret_t Set(const char* choose, void* val)override;
	virtual plc_api_ret_t Get(const char* choose, void* val)override;
	virtual plc_api_ret_t Run(GaapiData_c& iData, GaapiData_c& oData)override;
	virtual plc_api_ret_t Receive(GaapiData_c& iData)override;
	virtual plc_api_ret_t Generate(GaapiData_c& oData)override;
	virtual plc_api_ret_t Close()override;
public:
	GaapiGaf_c* _plcAlgo = 0;
	plcApiParam_t _param = { 0 };
};



#if 0
typedef struct plc_api_param_t {
#if 0
	int32_t fs_hz;
	int16_t channels;
	int16_t frame_samples;
	uint16_t channel_select;
	enum plc_api_data_type_e data_type;
	enum plc_api_mode_e mode;
	enum plc_api_param_set_e param_set;
	struct {
		int32_t key;
		enum plc_api_application_e application;
		union {
			struct {
				int32_t overlap_samples;
				int32_t hold_samples_after_lost;
				int32_t attenuate_samples_after_lost;
				int32_t gain_samples_after_no_lost;
				int32_t seek_samples;
				int32_t no_seek_samples;
				int32_t match_samples;
			}music_plc;
			struct {
				enum sbc_plc_codec_type_e codec_type;
				int32_t search_size;
				int32_t match_size;
				int32_t olal;
				int32_t recover;
			}sbc_plc;
			struct {
				int32_t recv;
			}ts_plc;
		};
	};
	void* (*cb_malloc)(int size);
	void (*cb_free)(void* ptr);
	void (*cb_printf)(const char* fmt, ...);
	void* other;
#endif
};
#endif

