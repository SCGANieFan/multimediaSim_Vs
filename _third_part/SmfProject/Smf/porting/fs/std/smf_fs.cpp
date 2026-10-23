#include "stdio.h"
#include <cstdint>
namespace smf {
	void* smf_media_fopen(const char* path, const char* mode, void* iopara) {
		return fopen(path, mode);
	}
	int smf_media_fclose(void* hd) {
		fclose((FILE*)hd);
		return 0;
	}
	int smf_media_fseek(void* hd, int64_t offset, int from) {
		return fseek((FILE*)hd, (int)offset, from);
	}
	int64_t smf_media_ftell(void* hd) {
		return ftell((FILE*)hd);
	}
	int smf_media_fread(void* hd, void* buff, unsigned size, void* iopara) {
		return fread(buff, 1, size, (FILE*)hd);
	}
	int smf_media_fwrite(void* hd, void* buff, unsigned size) {
		return fwrite(buff, 1, size, (FILE*)hd);
	}

	void* fsopen(const char* __filename, const char* __modes) {
		return fopen(__filename, __modes);
	}
	int fsclose(void* __stream) {
		fclose((FILE*)__stream);
		return 0;
	}
	size_t fsread(void* __ptr, size_t __size, size_t __n, void* __stream) {
		return fread(__ptr, __size, __n, (FILE*)__stream);
	}
	size_t fswrite(const void* __ptr, size_t __size, size_t __n, void* __stream) {
		return fwrite(__ptr, __size, __n, (FILE*)__stream);
	}
	int fsseek(void* __stream, long int __off, int __whence) {
		return fseek((FILE*)__stream, __off, __whence);
	}
	long fstell(void* __stream) {
		return ftell((FILE*)__stream);
	}
}