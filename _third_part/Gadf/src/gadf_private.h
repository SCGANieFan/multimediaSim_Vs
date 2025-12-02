#pragma once
#include<stdint.h>
#include<stddef.h>

#if WIN32
#define LOG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt, Strrchr_m(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt, Strrchr_m(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif


class GadfBase_c {
public:
	struct BasePort_t {
		void* (*_malloc)(int size) = 0;
		void (*_free)(void* ptr) = 0;
		void (*_print)(const char* fmt, ...) = 0;
	};
public:
	GadfBase_c() {}
	~GadfBase_c() {}
public:
	static void* operator new(size_t size, void* buf) { return buf; }
	static void operator delete(void* buf, size_t size) { return; }
	static void operator delete(void* buf, void* place) { return; }
	static void operator delete(void* buf) { return; }
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init();
	virtual bool DeInit();
public:
	static constexpr uint64_t Str2Key(const char* str) {
		uint64_t key = 0;
		for (uint8_t n = 0; n < 8; n++) {
			if (str[n] == '\0') {
				break;
			}
			key = (key << 8) | str[n];
		}
		return key;
	}
	static constexpr char* Strrchr_m(const char* str, char c) {
		int32_t strLen = 0;
		while (str[strLen++] != '\0');
		strLen -= 1;
		char* outStr = (char*)str;
		if (c == '\0') {
			return outStr + strLen + 1;
		}
		for (int32_t n = strLen; n > 0; n--) {
			if (outStr[n] == c) {
				return (outStr + n);
			}
		}
		return outStr;
	}
public:
	BasePort_t _bp;
};

class GadfData_c:public GadfBase_c {
public:
	GadfData_c() {}
	~GadfData_c() {}
public:
	void* Data() { return (uint8_t*)_buf + _offset; }
	void* LeftData() { return (uint8_t*)_buf + _offset + _size; }
	uint32_t LeftSize() { return _max - _offset - _size; }
public:
	void* _buf = 0;
	uint32_t _size = 0;
	uint32_t _max = 0;
	uint32_t _offset = 0;
	uint32_t _flag = 0;
};

struct GadfAudioInfo_t {
	uint32_t rate = 0;
	uint16_t ch = 0;
	uint16_t width = 0;
};

class GadfAudioData_c :public GadfData_c, public GadfAudioInfo_t {
public:
	GadfAudioData_c() {}
	~GadfAudioData_c() {}
public:
};

class GadfSource_c :public GadfBase_c {
public:
	GadfSource_c() {}
	~GadfSource_c() {}
public:
	virtual bool Init()override { return true; };
	virtual bool Generate(GadfData_c& oData) { return true; }
};

class GadfSink_c :public GadfBase_c {
public:
	GadfSink_c() {}
	~GadfSink_c() {}
public:
	virtual bool Init()override { return true; };
	virtual bool Receive(GadfData_c& iData) { return true; }
};

class GadfAlgo_c :public GadfBase_c {
public:
	GadfAlgo_c() {}
	~GadfAlgo_c() {}
public:
	virtual bool Init()override { return true; };
	virtual bool Process(GadfData_c& iData, GadfData_c& oData) { return true; }
};

class Gadf_c :public GadfBase_c {
public:
	Gadf_c();
	~Gadf_c();
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init()override;
	virtual bool DeInit()override;
	virtual void Run();
public:
	GadfSource_c* _source = 0;
	GadfSink_c* _sink = 0;
	GadfAlgo_c* _algo = 0;
};


class GadfSourceArray_c :public GadfSource_c {
public:
	GadfSourceArray_c() {}
	~GadfSourceArray_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init()override;
	virtual bool Generate(GadfData_c& oData) override;
	virtual bool DeInit();
public:
	uint8_t* _buf = 0;
	uint32_t _bufByteMax = 0;
};

class GadfSourceFile_c :public GadfSource_c {
public:
	GadfSourceFile_c() {}
	~GadfSourceFile_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init()override;
	virtual bool Generate(GadfData_c& oData) final;
	virtual bool DeInit();
public:
	void* _fp = 0;
	const char* _url = 0;
	void* _buf = 0;
	uint32_t _bufByteMax = 0;
};


class GadfSinkArray_c :public GadfSink_c {
public:
	GadfSinkArray_c() {}
	~GadfSinkArray_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init()override;
	virtual bool Receive(GadfData_c& iData);
	virtual bool DeInit();
public:
	void* _buf = 0;
	uint32_t _bufByteMax = 0;
	uint32_t _bufByte = 0;
};

class GadfSinkFile_c :public GadfSink_c {
public:
	GadfSinkFile_c() {}
	~GadfSinkFile_c() {}
public:
	virtual bool Set(const char* key, void* val);
	virtual bool Init()override;
	virtual bool Receive(GadfData_c& iData);
	virtual bool DeInit();
public:
	const char* _url = 0;
	uint32_t _bufByte = 0;
	void* _fp = 0;
};
