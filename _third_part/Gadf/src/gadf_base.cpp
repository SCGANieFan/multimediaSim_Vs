#include <string.h>
#include "gadf_base.h"
#include "gadf_private.h"
#include "gadf_porting_api.h"

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


Gadf_c::Gadf_c() {
	LOG("");
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
	//LOG("");
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
	//LOG("");
	ret |= _source->DeInit();
	ret |= _sink->DeInit();
	ret |= _algo->DeInit();
	if (!ret) return false;
	return true;
}
void Gadf_c::Run() {
	//LOG("");
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
	LOG("");
	if (!oData.Buf()) {
		oData.Init(_buf, _bufByteMax, _bufByteMax);
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
	case Str2Key("fByte"):
		_fByte = (uint32_t)val; return true;
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
		LOG("fopen fail, %s", _url);
		return false;
	}
	_buf = _bp._malloc(_fByte);
	if (!_buf) {
		LOG("malloc fail, %p,%d", _buf, _fByte);
		return false;
	}
	LOG("%p,%d,%s", _buf, _fByte, _url);
	return true;
}
bool GadfSourceFile_c::Generate(GadfData_c& oData) {
	if (!oData.Buf()) {
		oData.Init(_buf, _fByte);
	}
	oData.Clear();
	uint32_t readByte = GadfFileRead(_fp, oData.LeftData(), oData.LeftSize());
	if (readByte < oData.LeftSize()) {
		//oData._flag = 1;
		return false;
	}
	oData.Append(readByte);
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
	_fByte = 0;
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
	_bufByte = 0;
	return true;
}
bool GadfSinkArray_c::Receive(GadfData_c& iData) {
	if (!_buf) return false;
	uint32_t iSize = iData.Size();
	if (iSize) {
		if ((iSize + _bufByte) > _bufByteMax) {
			return false;
		}
		memcpy((uint8_t*)_buf + _bufByte, iData.Data(), iSize);
		_bufByte += iSize;
		iData.Used(iSize);
		iData.Clear();
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
			LOG("open fail, %s", _url);
			return false;
		}
	}
	return true;
}
bool GadfSinkFile_c::Receive(GadfData_c& iData) {
	if (!_fp) {
		return false;
	}
	if (iData.Size()) {
		GadfFileWrite(_fp, iData.Data(), iData.Size());
		iData.Used();
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

