#pragma once
#include<stdint.h>

namespace MTFApi_ns {
#define MTF_REGISTER_FUNC(name) mtf_##name##_register()
#define MTF_REGISTER(name) extern void MTF_REGISTER_FUNC(name);MTF_REGISTER_FUNC(name);

class MTFApi{
public:
	MTFApi() {}
	~MTFApi() {}
public:
	static int32_t Init();
	static int32_t Api(const char* str, void** param);
};
}