#pragma once
#include <stdint.h>
class plc_base_port_c {
public:
	plc_base_port_c() {}
	~plc_base_port_c() {}
public:
	virtual void* malloc(int32_t size) = 0;
	virtual void free(void* block) = 0;
public:
	void (*print_cb)(const char* fmt, ...) = 0;
};
