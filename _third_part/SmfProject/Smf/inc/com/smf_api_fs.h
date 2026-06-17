#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

EXTERNC void smf_fs_register(void);

//EXTERNC void smf_file_buffer_register(const char* filename, void* data, int size);

typedef void* (*CbFOpen)(const char* __filename, const char* __modes);
typedef int (*CbFClose)(void* __stream);
typedef size_t(*CbFRead)(void* __ptr, size_t __size, size_t __n, void* __stream);
typedef size_t(*CbFWrite)(const void* ptr, size_t size, size_t nmemb, void* __stream);
typedef int (*CbFSeek)(void* __stream, long int __off, int __whence);
typedef long (*CbFTell)(void* __stream);
typedef long (*CbFLength)(void* __stream);
EXTERNC void smf_fs_callback_register(
	CbFOpen fopen
	, CbFClose fclose
	, CbFRead fread
	, CbFWrite fwrite
	, CbFSeek fseek
	, CbFTell ftell
	, CbFLength flength
);

EXTERNC void smf_io_callback_register(const char* io_name
	, CbFOpen fopen
	, CbFClose fclose
	, CbFRead fread
	, CbFWrite fwrite
	, CbFSeek fseek
	, CbFTell ftell
	, CbFLength flength
);

EXTERNC void smf_io_buffer_list_register(const char* io_name);
EXTERNC void smf_io_buffer_item_register(const char* url, void* data, uint32_t size);

EXTERNC FILE* iofopen(const char* filename, const char* mode);
EXTERNC size_t iofread(void* ptr, size_t size, size_t count, FILE* stream);
EXTERNC size_t iofwrite(const void* ptr, size_t size, size_t count, FILE* stream);
EXTERNC int iofclose(FILE* stream);
EXTERNC int iofseek(FILE* stream, long offset, int whence);
EXTERNC long ioftell(FILE* stream);

EXTERNC void smf_folder_scan(const char* lpPath, void(*cb)(const char* fname, void* priv), bool recursion, char skipFolderPrefix, void* priv);
EXTERNC void smf_print_file_hash(const char* path);
