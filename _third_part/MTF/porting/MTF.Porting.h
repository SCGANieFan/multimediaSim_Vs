#pragma once
#include"MTF.Type.h"
#include"MTF.Std.h"

// std
mtf_i32 VsprintfPorting(char* const buffer, char const* const format, VaListPorting_t argList);
mtf_i32 VsprintfPorting(char* const buffer, char const* const format, ...);
mtf_i32 VsnprintfPorting(char* const buffer, mtf_i32 const bufferCount, char const* const format, char* argList);
mtf_i32 SnprintfPorting(char* const buffer, mtf_i32 const bufferCount, char const* const format, ...);

// log
#if 0
mtf_void LogPorting(const char* format, va_list args);
//mtf_void LogPorting(const char* buf);
#endif
mtf_void LogPorting(const char* format, ...);
mtf_void LogNoFormatPorting(const char* buf);



// file
typedef void FilePorting_t;
enum FileSeekPorting_e
{
	FILE_PORTING_SEEK_CUR = 1,
	FILE_PORTING_SEEK_END = 2,
	FILE_PORTING_SEEK_SET = 0
};
FilePorting_t* FileOpenPorting(const char* url, const char* mode);
mtf_i32 FileReadPorting(FilePorting_t* hd, mtf_void* buf, mtf_i32 bufByte);
mtf_i32 FileWritePorting(FilePorting_t* hd, mtf_void* buf, mtf_i32 bufByte);
mtf_i32 FileSeekPorting(FilePorting_t* hd, mtf_i32 off, FileSeekPorting_e seek);
mtf_i32 FileTellPorting(FilePorting_t* hd);
mtf_i32 FileClosePorting(FilePorting_t* hd);

// time
mtf_i64 TimeMsPorting();
mtf_i64 TimeUsPorting();
mtf_void SleepMsPorting(mtf_i32 ms);

// thread
mtf_u32 ThreadIdPorting();