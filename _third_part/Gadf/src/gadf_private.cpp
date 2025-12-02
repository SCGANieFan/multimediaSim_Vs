#include <string.h>
#include "gadf_private.h"
#include "gadf_porting_api.h"
#include "hal_trace.h"
#include "heap_api.h"

bool GadfBase_c::Set(const char* key, void* val) {
	uint64_t _key = Str2Key(key);
	switch (_key)
	{
	case Str2Key("basePort"):
		_bp = *(BasePort_t*)val; return true;
	default:
		break;
	}
	return false;
}
bool GadfBase_c::Init() {
	return true;
}
bool GadfBase_c::DeInit() {
	return true;
}


Gadf_c::Gadf_c() {
	LOG(_bp._print, "");
}
Gadf_c::~Gadf_c() {}

bool Gadf_c::Set(const char* key, void* val) {
	uint64_t key0 = Str2Key(key);
	switch (key0)
	{
	case Str2Key("source"):
		_source = (GadfSource_c*)val; return true;
	case Str2Key("sink"):
		_sink = (GadfSink_c*)val; return true;
	case Str2Key("algo"):
		_algo = (GadfAlgo_c*)val; return true;
	default:
		break;
	}
	bool ret = false;
	ret = GadfBase_c::Set(key, val);
	if (!ret && _source)ret = _source->Set(key, val);
	if (!ret && _sink)ret = _sink->Set(key, val);
	if (!ret && _algo)ret = _algo->Set(key, val);
	return ret;
}

bool Gadf_c::Init() {
	bool ret = GadfBase_c::Init();
	LOG(_bp._print, "");
	if (!ret) return false;
	_source->Set("basePort", &_bp);
	_sink->Set("basePort", &_bp);
	_algo->Set("basePort", &_bp);
	ret |= _source->Init();
	ret |= _sink->Init();
	ret |= _algo->Init();
	if (!ret) return false;
	return true;
}
bool Gadf_c::DeInit() {
	bool ret = GadfBase_c::DeInit();
	if (!ret) return false;
	LOG(_bp._print, "");
	ret |= _source->DeInit();
	ret |= _sink->DeInit();
	ret |= _algo->DeInit();
	if (!ret) return false;
	return true;
}
void Gadf_c::Run() {
	LOG(_bp._print, "");
	bool ret = true;
	GadfData_c iData;
	GadfData_c oData;
	while (1) {
		ret = _source->Generate(iData);
		if (!ret) break;
		ret = _algo->Process(iData, oData);
		if (!ret) break;
		ret = _sink->Receive(oData);
		if (!ret) break;
	}
}

bool GadfSourceArray_c::Set(const char* key, void* val) {
	switch (Str2Key(key))
	{
	case Str2Key("buf"):
		_buf = (uint8_t*)(uint32_t)val; return true;
	case Str2Key("bufMax"):
		_bufByteMax = (uint32_t)val; return true;
	default:
		break;
	}
	return GadfSource_c::Set(key, val);
}

bool GadfSourceArray_c::Init() {
	if (!GadfSource_c::Init())
		return false;
	return true;
}
bool GadfSourceArray_c::Generate(GadfData_c& oData) {
	LOG(_bp._print, "");
	if (!oData._buf) {
		oData._buf = _buf;
		oData._size = oData._max = _bufByteMax;
	}
	if (oData._size == 0) {
		return false;
	}
	return true;
}
bool GadfSourceArray_c::DeInit() {
	if (!GadfSource_c::DeInit())
		return false;
	return true;
}


bool GadfSourceFile_c::Set(const char* key, void* val) {
	switch (Str2Key(key))
	{
	case Str2Key("url"):
		_url = (const char*)(uint32_t)val; return true;
	default:
		break;
	}
	return GadfSource_c::Set(key, val);
}
bool GadfSourceFile_c::Init() {
	if (!GadfSource_c::Init())
		return false;
	_fp = GadfFileOpen(_url, "rb");
	if (!_fp) {
		LOG(_bp._print, "open fail, %s", _url);
		return false;
	}
	GadfFileSeek(_fp, 0, GADF_FILE_SEEK_END);
	_bufByteMax = GadfFileTell(_fp);
	_buf = _bp._malloc(_bufByteMax);
	if (!_buf) {
		LOG(_bp._print, "open fail, %p,%d,%s", _buf, _bufByteMax, _url);
		return false;
	}
	LOG(_bp._print, "%p,%d,%s", _buf, _bufByteMax, _url);
	return true;
}
bool GadfSourceFile_c::Generate(GadfData_c& oData) {
	if (!oData._buf) {
		oData._buf = _buf;
		oData._size = oData._max = _bufByteMax;
	}
	if (oData.LeftSize() == 0) {
		return false;
	}
	return true;
}
bool GadfSourceFile_c::DeInit() {
	if (_fp) {
		GadfFileClose(_fp);
		_fp = 0;
	}
	if (_buf) {
		_bp._free(_buf);
		_buf = 0;
	}
	_bufByteMax = 0;
	if (!GadfSource_c::DeInit())
		return false;
	return true;
}

bool GadfSinkArray_c::Set(const char* key, void* val) {
	switch (Str2Key(key))
	{
	case Str2Key("buf"):
		_buf = (void*)(uint32_t)val; return true;
	case Str2Key("bufMax"):
		_bufByteMax = (uint32_t)val; return true;
	default:
		break;
	}
	return GadfSink_c::Set(key, val);
}
bool GadfSinkArray_c::Init() {
	if (!GadfSink_c::Init())
		return false;
	return true;
}
bool GadfSinkArray_c::Receive(GadfData_c& iData) {
	if (!_buf) return false;
	if (iData._size) {
		if ((iData._size + _bufByte) > _bufByteMax) {
			return false;
		}
		memcpy((uint8_t*)_buf + _bufByte, iData.Data(), iData._size);
		iData._size = 0;
		iData._offset = 0;
	}
	return true;
}
bool GadfSinkArray_c::DeInit() {
	if (!GadfSink_c::DeInit())
		return false;
	return true;
}

bool GadfSinkFile_c::Set(const char* key, void* val) {
	switch (Str2Key(key))
	{
	case Str2Key("url"):
		_url = (const char*)(uint32_t)val; return true;
	default:
		break;
	}
	return GadfSink_c::Set(key, val);
}
bool GadfSinkFile_c::Init() {
	if (!GadfSink_c::Init())
		return false;
	if (!_fp) {
		_fp = GadfFileOpen(_url, "wb");
		if (!_fp) {
			LOG(_bp._print, "open fail, %s", _url);
			return false;
		}
	}
	return true;
}
bool GadfSinkFile_c::Receive(GadfData_c& iData) {
	if (!_fp) {
		return false;
	}
	if (iData._size) {
		GadfFileWrite(_fp, iData.Data(), iData._size);
		_bufByte += iData._size;
		iData._size -= iData._size;
	}
	return true;
}
bool GadfSinkFile_c::DeInit() {
	if (_fp) {
		GadfFileClose(_fp);
	}
	if (!GadfSink_c::DeInit())
		return false;
	return true;
}

