#include <stdio.h>
#include "MTF.Porting.h"

FilePorting_t* FileOpenPorting(const char* url, const char* mode) {
	return (FilePorting_t*)fopen(url, mode);
}
mtf_i32 FileReadPorting(FilePorting_t* hd, mtf_void* buf, mtf_i32 bufByte) {
	return fread(buf, 1, bufByte, (FILE*)hd);
}
mtf_i32 FileWritePorting(FilePorting_t* hd, mtf_void* buf, mtf_i32 bufByte) {
	return fwrite(buf, 1, bufByte, (FILE*)hd);
}
mtf_i32 FileSeekPorting(FilePorting_t* hd, mtf_i32 off, FileSeekPorting_e seek) {
	return fseek((FILE*)hd, off, seek);
}
mtf_i32 FileTellPorting(FilePorting_t* hd) {
	return ftell((FILE*)hd);
}
mtf_i32 FileClosePorting(FilePorting_t* hd) {
	return fclose((FILE*)hd);
}

