#pragma once
#include <new>
#include "OggBase.h"

namespace ogg_ns {

class OggMemoryManger_c
{
public:
	OggMemoryManger_c() {}
	~OggMemoryManger_c() {}
public:
	void Init(OggBasePorting_t *base_porting);
	void* malloc(int32_t size);
	void* realloc(void *bufOri, int32_t size);
	void free(void *ptr);
	void FreeAll();
	template<class T>
	void* New(){
		void *ptr = malloc(sizeof(T));
		if(!ptr)
			return 0;
		return new(ptr) T();
	}
	template<class T>
	void Delete(void *ptr){
		((T*)ptr)->~T();
		free(ptr);
	}
private:
	OggBasePorting_t* _basePorting;
	void* _allocList[100];
};
};




