#pragma once
#include "gadf.h"
#include "gadf_base.h"

class GadfOggMuxSourceArray_c :public GadfSourceArray_c {
public:
	GadfOggMuxSourceArray_c() {}
	~GadfOggMuxSourceArray_c() {}
public:
	virtual bool Set(const char* key, void* val) override;
	virtual bool Generate(GadfData_c& oData) override;
public:
	const char *_type = 0;
	uint32_t _bufUsedByte = 0;
	bool _isEos = false;
};

class GadfOggMuxSourceFile_c :public GadfSourceFile_c {
public:
	GadfOggMuxSourceFile_c() {}
	~GadfOggMuxSourceFile_c() {}
};

class GadfOggSinkArray_c :public GadfSinkArray_c {
public:
	GadfOggSinkArray_c() {}
	~GadfOggSinkArray_c() {}
};

class GadfOggSinkFile_c :public GadfSinkFile_c {
public:
	GadfOggSinkFile_c() {}
	~GadfOggSinkFile_c() {}
public:
 	virtual bool Receive(GadfData_c& iData)override;
	virtual bool Set(const char* key, void* val)override;
	virtual bool Init()override;
	virtual bool DeInit()override;
public:
	uint32_t _fs = 0;
	uint8_t _ch = 0;
	uint8_t _width = 0;
	uint32_t _frame0p1Ms = 200;
	void* _ogg = 0;
	GadfData_c oData;
};

class GadfOggMux_c :public GadfAlgo_c {
public:
	GadfOggMux_c() {}
	~GadfOggMux_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init();
	virtual bool Process(GadfData_c& iData, GadfData_c& oData);
	virtual bool DeInit();
public:
	void* _oBuf = 0;
	uint32_t _oBufMax = 0;
};


class GadfOggDemo_c :public Gadf_c {
public:
	GadfOggDemo_c() {}
	~GadfOggDemo_c() {}
};


