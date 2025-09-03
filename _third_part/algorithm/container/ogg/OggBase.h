#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <new.h>
#include "ogg_api.h"

namespace ogg_ns {

#if WIN32
#define LOG_OGG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_OGG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif

#define OGG_VERSION "1.0.0"

	typedef struct {
		void* (*malloc_cb)(uint32_t);
		void* (*realloc_cb)(void*, uint32_t);
		void (*free_cb)(void*);
		void (*printf_cb)(const char*, ...);
	}OggBasePorting_t;
};
