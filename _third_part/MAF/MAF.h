#pragma once
#if 0
#include<stdint.h>

typedef void* (*Malloc_t)(int32_t size);
typedef void* (*Realloc_t)(void* block, int32_t size);
typedef void* (*Calloc_t)(int32_t count, int32_t size);
typedef void (*Free_t)(void* block);


typedef int32_t MA_Ret;


#define MA_RET_SUCCESS 0
#define MA_RET_FAIL -1
#define MA_RET_INPUT_ERROR -2



#define MAF_REGISTER(name) \
extern void maf_##name##_register();\
			maf_##name##_register();


#define EXTERNC extern "C"

#if 1
typedef enum {
	AA_DataFlag_FRAME_IS_EMPTY = 1 << 0,
	AA_DataFlag_FRAME_IS_EOS = 1 << 1,
	AA_DataFlag_FRAME_IS_EXTRA_INFO = 1 << 2,
}AA_DataFlag;
#endif


typedef struct {
	uint8_t* buff;
	int32_t off;
	int32_t size;
	int32_t max;
	uint32_t flags;
}AA_Data;



//hd
EXTERNC MA_Ret MAF_GetHandleSize(const char* type, int32_t* size);
EXTERNC MA_Ret MAF_Init(void* hd, const char* script, void** param);
EXTERNC MA_Ret MAF_Set(void* hd, const char* key,void** val);
EXTERNC MA_Ret MAF_Get(void* hd, const char* key,void** val);
EXTERNC MA_Ret MAF_Run(void* hd, AA_Data* dataIn, AA_Data* dataOut);
EXTERNC MA_Ret MAF_Receive(void* hd, AA_Data* dataIn);
EXTERNC MA_Ret MAF_Generate(void* hd, AA_Data* dataOut);
EXTERNC MA_Ret MAF_Deinit(void* hd);


#endif