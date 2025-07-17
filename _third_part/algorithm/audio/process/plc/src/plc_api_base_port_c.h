#pragma once
#include "plc_base_port_c.h"
class plc_api_base_port_c : public plc_base_port_c {
public:
	plc_api_base_port_c() {}
	~plc_api_base_port_c() {}
#if 0
public:
	void operator=(plc_api_base_port_c& src) {
		this->malloc_cb = src.malloc_cb;
		this->free_cb= src.free_cb;
		this->print_cb = src.print_cb;
	}
#endif
public:
	void *malloc(int size){
		return malloc_cb(size);
	}
	void free(void *ptr){
		free_cb(ptr);
	}
public:
	void *(*malloc_cb)(int size) = 0;
	void (*free_cb)(void *ptr) = 0;
};
