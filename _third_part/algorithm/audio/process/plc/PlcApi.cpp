#if 1
#include<new>
#include"PlcApi.h"
#include"Algo.Memory.h"
using namespace Algo;

#ifdef WIN32
#define USE_MUSIC_PLC 1
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt "\n",__func__, __LINE__, ##__VA_ARGS__)
#define PLC_DATA_TYPE_SHORT_16	(1)
#define PLC_DATA_TYPE_INT_32	(1)
#define PLC_DATA_TYPE_FLOAT_32  (1)
#else
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

#if USE_MUSIC_PLC
#include"MusicPlc.h"
#endif

enum PlcApiKey_e {
	PLC_API_KEY_HM_CAST_AP_SLEEP = 0,
};


class PlcApiBasePort_c : public AlgoBasePorting_c {
public:
	PlcApiBasePort_c() {}
	~PlcApiBasePort_c() {}
public:
	void *Malloc(int size){
		return malloc_cb(size);
	}
	void Free(void *ptr){
		free_cb(ptr);
	}
public:
	void *(*malloc_cb)(int size);
	void (*free_cb)(void *ptr);
};


class PlcApiCom_c {
public:
	PlcApiCom_c() {}
	~PlcApiCom_c() {}
public:
	virtual PlcApiRet Create(void** pHd, PlcApiParam_t* param, PlcApiBasePort_c *plcMemory) const {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) const {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Set(void* hd, PlcApiSet_e choose, void* val) const {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Get(void* hd, PlcApiGet_e choose, void* val) const {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Destory(void* hd) const {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual b1 IsSupport() const {
		return false;
	}
};

template<class T>
class PlcApiMusic_c : public PlcApiCom_c {
public:
	PlcApiMusic_c() {}
	~PlcApiMusic_c() {}
#if USE_MUSIC_PLC
public:
	virtual PlcApiRet Create(void** pHd, PlcApiParam_t* param, PlcApiBasePort_c* plcMemory) const {
		*pHd = 0;
		MusicPlc_ns::MusicPlcParam_t musicplcParam;
		ALGO_MEM_SET(&musicplcParam, 0, sizeof(MusicPlc_ns::MusicPlcParam_t));
		musicplcParam.basePorting = (AlgoBasePorting_c*)plcMemory;
		musicplcParam.fsHz = param->fsHz;
		musicplcParam.channels = param->channels;
		musicplcParam.width = param->width;
		musicplcParam.frameSamples = param->frameSamples;

		musicplcParam.overlapSamples = 2 * musicplcParam.fsHz / 1000;
		musicplcParam.holdSamplesAfterLost = 0 * musicplcParam.fsHz / 1000;
		musicplcParam.attenuateSamplesAfterLost = 30 * musicplcParam.fsHz / 1000;
		musicplcParam.gainSamplesAfterNoLost = 30 * musicplcParam.fsHz / 1000;
		musicplcParam.seekSamples = 10 * musicplcParam.fsHz / 1000;
		musicplcParam.noSeekSamples = 5 * musicplcParam.fsHz / 1000;
		musicplcParam.matchSamples = 2 * musicplcParam.fsHz / 1000;
		if (param->key == PLC_API_KEY_HM_CAST_AP_SLEEP) {
			switch (param->application) {
			case PLC_API_APPLICATION_MUSIC:
				musicplcParam.overlapSamples = 2 * musicplcParam.fsHz / 1000;
				musicplcParam.holdSamplesAfterLost = 0 * musicplcParam.fsHz / 1000;
				musicplcParam.attenuateSamplesAfterLost = 50 * musicplcParam.fsHz / 1000;
				musicplcParam.gainSamplesAfterNoLost = 50 * musicplcParam.fsHz / 1000;
				musicplcParam.seekSamples = 10 * musicplcParam.fsHz / 1000;
				musicplcParam.noSeekSamples = 10 * musicplcParam.fsHz / 1000;
				musicplcParam.matchSamples = 2 * musicplcParam.fsHz / 1000;
				break;
			case PLC_API_APPLICATION_192K:
				musicplcParam.overlapSamples = 2 * musicplcParam.fsHz / 1000;
				musicplcParam.holdSamplesAfterLost = 0 * musicplcParam.fsHz / 1000;
				musicplcParam.attenuateSamplesAfterLost = 30 * musicplcParam.fsHz / 1000;
				musicplcParam.gainSamplesAfterNoLost = 30 * musicplcParam.fsHz / 1000;
				musicplcParam.seekSamples = 90 * musicplcParam.fsHz / 1000;
				musicplcParam.noSeekSamples = 5 * musicplcParam.fsHz / 1000;
				musicplcParam.matchSamples = 100 * musicplcParam.fsHz / 1000;
				break;
			}
		}
		LOG(param->cb_printf, "plc api musicplc, (%d,%d),(%p,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
			param->key,
			param->application,
			musicplcParam.basePorting,
			musicplcParam.fsHz,
			musicplcParam.channels,
			musicplcParam.width,
			musicplcParam.frameSamples,
			musicplcParam.overlapSamples,
			musicplcParam.holdSamplesAfterLost,
			musicplcParam.attenuateSamplesAfterLost,
			musicplcParam.gainSamplesAfterNoLost,
			musicplcParam.seekSamples,
			musicplcParam.noSeekSamples,
			musicplcParam.matchSamples);
		void* plc;
		int32_t ret = MusicPlc_ns::MusicPlc_c<T>::Create(&plc, &musicplcParam);
		if (!plc) {
			LOG(plcMemory->print_cb, "plc api create fail, %d", ret);
			return PLC_API_RET_FAIL;
		}
		MusicPlc_ns::MusicPlc_c<T>::Set(plc, MusicPlc_ns::MusicPlc_SetChhoose_e::MUSIC_PLC_SET_CHANNEL_SELECT, (void*)(int32_t)(param->channelSelect));
		*pHd = plc;
		return PLC_API_RET_SUCCESS;
	}
	virtual PlcApiRet Run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) const {
		int32_t ret = MusicPlc_ns::MusicPlc_c<T>::Run(hd, in, inLen, out, pOutLen, isLost);
		if (ret != MUSIC_PLC_API_RET_SUCCESS) {
			return PLC_API_RET_FAIL;
		}
		if (inUsed)
			*inUsed = inLen;
		return PLC_API_RET_SUCCESS;
	}
	virtual PlcApiRet Set(void* hd, PlcApiSet_e choose, void* val) const {
		return PLC_API_RET_SUCCESS;
	}
	virtual PlcApiRet Get(void* hd, PlcApiGet_e choose, void* val) const {
		switch (choose)
		{
		case PLC_API_GET_HEAP_SIZE: {
			int* heapSize = (int*)((void**)val)[0];
#if 0
			PlcParam* param = (PlcParam*)((void**)val)[1];
			void* pp[] = {
				(void*)param->fsHz,
				(void*)param->channels,
				(void*)param->width,
				(void*)param->frameSamples,
				(void*)param->MusicPlcParam.overlapSamples,
				(void*)param->MusicPlcParam.holdSamples,
				(void*)param->MusicPlcParam.fadeSamples,
				(void*)param->MusicPlcParam.gainSamples,
				(void*)param->MusicPlcParam.seekSamples,
				(void*)param->MusicPlcParam.matchSamples,
				param,
			};
			MusicPlc_ns::MusicPlc_c<T>::Get(0, MusicPlc_GetChhoose_e::MUSIC_PLC_GET_HEAP_SIZE, &pp);
#endif
			*heapSize = 55 * 1024;
			break;
		}
		case PLC_API_GET_MAX:
			return PLC_API_RET_SUCCESS;
			break;
		default:
			break;
		}
		return PLC_API_RET_SUCCESS;
	}
	virtual PlcApiRet Destory(void* hd) const {
		MusicPlc_ns::MusicPlc_c<T>::Destory(hd);
		return PLC_API_RET_SUCCESS;
	}
	virtual b1 IsSupport() const {
		return true;
	}
#endif
};

template<>
class PlcApiMusic_c<bool> : public PlcApiCom_c {
public:
	PlcApiMusic_c() {}
	~PlcApiMusic_c() {}
};



class PlcApi_c {
public:
	PlcApi_c() {}
	~PlcApi_c() {}
public:
	static inline PlcApiRet Create(void** pHd, PlcApiParam_t* param) {
		if (!pHd
			|| !param)
			return PLC_API_RET_INPUT_ERROR;
		LOG(param->cb_printf, "plc api (%s) (%p,%p) (%d,%d)",
			version,
			pHd, param, 
			param->mode, param->dataType);
		if (param->width == 2) {
			if (param->dataType != PlcApiDataType_e::PLC_API_DATA_TYPE_SHORT_16) {
				return PLC_API_RET_INPUT_ERROR;
			}
		}
		else if (param->width == 4) {
			if (param->dataType != PlcApiDataType_e::PLC_API_DATA_TYPE_INT_32
				&& param->dataType != PlcApiDataType_e::PLC_API_DATA_TYPE_FLOAT_32) {
				return PLC_API_RET_INPUT_ERROR;
			}
		}
		else {
			return PLC_API_RET_INPUT_ERROR;
		}
		int32_t size = sizeof(PlcApi_c);
		PlcApi_c* plcApi = (PlcApi_c*)param->cb_malloc(size);
		if (!plcApi) {
			LOG(param->cb_printf, "plc api malloc fail, (%p,%d)", param->cb_malloc, size);
			return PLC_API_RET_FAIL;
		}
		new(plcApi) PlcApi_c();
		LOG(param->cb_printf, "plc api (%d,%d,%d)",
			plcApi->plcApiMusic16b.IsSupport(), plcApi->plcApiMusic32b.IsSupport(), plcApi->plcApiMusic32f.IsSupport());
		plcApi->_basePort.malloc_cb = param->cb_malloc;
		plcApi->_basePort.free_cb = param->cb_free;
		plcApi->_basePort.print_cb = param->cb_printf;

		switch (param->mode) {
		case PlcApiMode_e::PLC_API_MODE_MUSIC_PLC:
			if (param->dataType == PlcApiDataType_e::PLC_API_DATA_TYPE_SHORT_16) {
				plcApi->_plcCom = &plcApi->plcApiMusic16b;
			}
			else if (param->dataType == PlcApiDataType_e::PLC_API_DATA_TYPE_INT_32) {
				plcApi->_plcCom = &plcApi->plcApiMusic32b;
			}
			else if(param->dataType == PlcApiDataType_e::PLC_API_DATA_TYPE_FLOAT_32){
				plcApi->_plcCom = &plcApi->plcApiMusic32f;
			}
			else {
				return PLC_API_RET_FAIL;
			}
			break;
		default:
			LOG(plcApi->_basePort.print_cb, "plc api mode error, %d", param->mode);
			Destory(plcApi);
			return PLC_API_RET_FAIL;
			break;
		}
		PlcApiRet ret = plcApi->_plcCom->Create(&plcApi->_plc, param, &plcApi->_basePort);
		if (ret != PLC_API_RET_SUCCESS) {
			LOG(param->cb_printf, "plc api create fail, %d", ret);
			Destory(plcApi);
			return ret;
		}
		*pHd = plcApi;
		LOG(param->cb_printf, "plc api create success, (%p,%p)", plcApi, plcApi->_plc);
		return PLC_API_RET_SUCCESS;
	}

	static PlcApiRet Run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) {
		if (!hd)
			return PLC_API_RET_INPUT_ERROR;
		if (!isLost){
			if (!in
				|| inLen < 1)
				return PLC_API_RET_INPUT_ERROR;
		}
		if (!out)
			return PLC_API_RET_INPUT_ERROR;
		PlcApi_c *plcApi = (PlcApi_c*)hd;
		return plcApi->_plcCom->Run(plcApi->_plc, in, inLen, inUsed, out, pOutLen, isLost);
	}
	static PlcApiRet Set(void* hd, PlcApiSet_e choose, void* val) {
		if (!hd) {
			return PLC_API_RET_INPUT_ERROR;
		}
		PlcApi_c* plcApi = (PlcApi_c*)hd;
		LOG(plcApi->_basePort.print_cb, "plc api set, (%p,%d,%d)", hd, choose, val);
		return plcApi->_plcCom->Set(plcApi->_plc, choose, val);
	}
	static PlcApiRet Get(void* hd, PlcApiGet_e choose, void* val) {
		if (!hd) {
			return PLC_API_RET_INPUT_ERROR;
		}
		PlcApi_c* plcApi = (PlcApi_c*)hd;
		LOG(plcApi->_basePort.print_cb, "plc api get, (%p,%d,%d)", hd, choose, val);
		return plcApi->_plcCom->Get(plcApi->_plc, choose, val);
	}
	static PlcApiRet Destory(void* hd) {
		if (!hd)
			return PLC_API_RET_INPUT_ERROR;
		PlcApi_c* plcApi = (PlcApi_c*)hd;
		LOG(plcApi->_basePort.print_cb, "plc api destory");
		if(plcApi->_plc)
			plcApi->_plcCom->Destory(plcApi->_plc);
		PlcApiBasePort_c basePort = plcApi->_basePort;
		plcApi->~PlcApi_c();
		basePort.Free(hd);
		return PLC_API_RET_SUCCESS;
	}
public:
	constexpr static const char* version = "1.0.3.0";
	PlcApiBasePort_c _basePort;
	const PlcApiCom_c* _plcCom;
	void* _plc;
#if PLC_DATA_TYPE_SHORT_16
	PlcApiMusic_c<int16_t> plcApiMusic16b;
#else
	PlcApiMusic_c<bool> plcApiMusic16b;
#endif
#if PLC_DATA_TYPE_INT_32
	PlcApiMusic_c<int32_t> plcApiMusic32b;
#else
	PlcApiMusic_c<bool> plcApiMusic32b;
#endif
#if PLC_DATA_TYPE_FLOAT_32
	PlcApiMusic_c<float> plcApiMusic32f;
#else
	PlcApiMusic_c<bool> plcApiMusic32f;
#endif
};



EXTERNC{

PlcApiRet PlcApiCreate(void** pHd, PlcApiParam_t* param){
	return PlcApi_c::Create(pHd,param);
}

PlcApiRet PlcApiRun(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost){
	return PlcApi_c::Run(hd,in,inLen,inUsed,out,pOutLen,isLost);
}

PlcApiRet PlcApiSet(void* hd, PlcApiSet_e choose, void* val) {
	return PlcApi_c::Set(hd,choose,val);
}

PlcApiRet PlcApiGet(void* hd, PlcApiGet_e choose, void* val) {
	return PlcApi_c::Get(hd,choose,val);
}

PlcApiRet PlcApiDestory(void* hd){
	return PlcApi_c::Destory(hd);
}

}

#endif