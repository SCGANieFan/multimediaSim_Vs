#pragma once

#include <new>
#include "plc_base.h"
#include "plc_base_port_c.h"

class plc_memory_manger_c
{
public:
	plc_memory_manger_c() {};
	~plc_memory_manger_c() {};
public:
	void Init(plc_base_port_c*base_porting);
	void* malloc(i32 size);
	void free(void *ptr);
	void FreeAll ();

	template<class T>
	INLINE void* New(){
		void *ptr = malloc(sizeof(T));
		if(!ptr)
			return 0;
		return new(ptr) T();
	}
	template<class T>
	INLINE void Delete(void *ptr){
		((T*)ptr)->~T();
		free(ptr);
	}
private:
	plc_base_port_c* _basePorting;
	void* _allocList[100];
};






