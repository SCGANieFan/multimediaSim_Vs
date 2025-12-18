#pragma once
#include "gadf_private.h"
#include "gadf_base.h"
#include "gadf.h"


class GadfPlcSourceArray_c :public GadfSourceArray_c {
public:
	GadfPlcSourceArray_c() {}
	virtual ~GadfPlcSourceArray_c() {}
public:
	virtual bool Init()override;
	virtual bool Set(const char* key, void* val)override;
	virtual bool Generate(GadfData_c& oData) override;
public:
	uint32_t _frameNum = 0;
	uint32_t sim_lost_num = 0;
	uint32_t sim_lost_den = 10;
};

class GadfPlcSourceFile_c :public GadfSourceFile_c {
public:
	GadfPlcSourceFile_c() {}
	virtual ~GadfPlcSourceFile_c() {}
public:
};

class GadfPlcSinkArray_c :public GadfSinkArray_c {
public:
	GadfPlcSinkArray_c() {}
	virtual ~GadfPlcSinkArray_c() {}
public:
	virtual bool Receive(GadfData_c& iData)override;
};

class GadfPlcSinkFile_c :public GadfSinkFile_c {
public:
	GadfPlcSinkFile_c() {}
	virtual ~GadfPlcSinkFile_c() {}
public:
};

class GadfPlcAlgo_c :public GadfAlgo_c {
public:
	GadfPlcAlgo_c() {}
	virtual ~GadfPlcAlgo_c() {}
public:
	virtual bool Init()override;
	virtual bool Set(const char* key, void* val)override;
	virtual bool Process(GadfData_c& iData, GadfData_c& oData)override;
	virtual bool DeInit()override;
public:
	uint32_t _plc_id = 0;
	uint32_t _rate = 0;
	uint16_t _channels = 0;
	uint16_t _width = 0;
	uint32_t _frameSample = 0;
	uint32_t _f0p1Ms = 0;
	void* _oBuf = 0;
	uint32_t _oBufMax = 0;
};


class GadfPlc_c :public Gadf_c {
public:
	GadfPlc_c() {}
	virtual ~GadfPlc_c() {}
};
