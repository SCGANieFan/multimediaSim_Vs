#pragma once
#include "gadf_info.h"

class GadfOpusDecSourceArray_c :public GadfSourceArray_c {
public:
	GadfOpusDecSourceArray_c() {}
	~GadfOpusDecSourceArray_c() {}
public:
	virtual bool Generate(GadfData_c& oData) override;
public:
	uint32_t _bufUsedByte = 0;
};

class GadfOpusDecSourceFile_c :public GadfSourceFile_c {
public:
	GadfOpusDecSourceFile_c() {}
	~GadfOpusDecSourceFile_c() {}
};

class GadfOpusEncSourceArray_c :public GadfSourceArray_c {
public:
	GadfOpusEncSourceArray_c() {}
	~GadfOpusEncSourceArray_c() {}
public:
	virtual bool Set(const char* key, void* val) override;
	virtual bool Generate(GadfData_c& oData) override;
public:
	uint32_t _fByte = 0;
	uint32_t _bufUsedByte = 0;
};

class GadfOpusEncSourceFile_c :public GadfSourceFile_c {
public:
	GadfOpusEncSourceFile_c() {}
	~GadfOpusEncSourceFile_c() {}
};

class GadfOpusSinkArray_c :public GadfSinkArray_c {
public:
	GadfOpusSinkArray_c() {}
	~GadfOpusSinkArray_c() {}
};

class GadfOpusSinkFile_c :public GadfSinkFile_c {
public:
GadfOpusSinkFile_c() {}
~GadfOpusSinkFile_c() {}
// public:
// 	virtual bool Receive(GadfData_c& iData)override;

};

class GadfOpusEnc_c :public GadfAlgo_c {
public:
	GadfOpusEnc_c() {}
	~GadfOpusEnc_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init();
	virtual bool Process(GadfData_c& iData, GadfData_c& oData);
	virtual bool DeInit();
public:
	void* _oBuf = 0;
	uint32_t _oBufMax = 0;

	uint32_t _fs = 0;
	uint8_t _ch = 0;
	uint8_t _width = 0;
	uint32_t _bitRate = 12000;
	uint32_t _frame0p1Ms = 200;
	bool _isWithHead = false;

	void* _enc = 0;
};

class GadfOpusDec_c :public GadfAlgo_c {
public:
	GadfOpusDec_c() {}
	~GadfOpusDec_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init();
	virtual bool Process(GadfData_c& iData, GadfData_c& oData);
	virtual bool DeInit();
public:
	void* _oBuf = 0;
	uint32_t _oBufMax = 0;

	uint32_t _fs = 0;
	uint8_t _ch = 0;
	uint8_t _width = 0;
	void* _dec = 0;
	bool _isWithHead = false;



	uint32_t _bitRate = 12000;
	uint32_t _frame0p1Ms = 200;

	uint32_t _bytePerFrame = 0;
	uint32_t _samplePerFrameOpusApiRet_t = 0;
};

class GadfOpusDemo_c :public Gadf_c {
public:
	GadfOpusDemo_c() {}
	~GadfOpusDemo_c() {}
};


