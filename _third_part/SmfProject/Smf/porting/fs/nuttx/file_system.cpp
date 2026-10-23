#include "smf_debug.h"
#include "smf_api_fs.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace smf {
	static int fopen_mode_to_open_oflag(const char* mode, int* optr)
    {
        int ret = 0, m = 0, o = 0;

        switch (*mode) {
        case 'r':
            m = O_RDONLY;
            o = 0;
            ret = 1;
            break;
        case 'w':
            m = O_WRONLY;
            o = O_CREAT | O_TRUNC;
            ret = 2;
            break;
        case 'a':
            m = O_WRONLY;
            o = O_CREAT | O_APPEND;
            ret = 3;
            break;
        default:
            return 0;
            break;
        }

        if (mode[1] == '+' || (mode[1] == 'b' && mode[2] == '+') || (mode[1] == '+' && mode[2] == 'b'))
        {
            ret |= 0x10;
            m = O_RDWR;
        }
        *optr = m | o;

        return (ret);
    }
	void* smf_posix_to_cmsis_open(const char* __filename, const char* __modes){
		int oflags = 0;
        if (fopen_mode_to_open_oflag(__modes, &oflags) == 0)return NULL;
        int fp = open(__filename, oflags, 0666);
        if(fp == -1){
            perror("open failed");
            return NULL;
        }
        returnIfErrCS(0, !fp, "fp is 0, filename: %s", __filename);
        return (void*)fp;
	}
	int smf_posix_to_cmsis_close(void* __stream){
        int fp = (int)__stream;
        return close(fp);
    }
	size_t smf_posix_to_cmsis_read(void* __ptr, size_t __size, size_t __n, void* __stream){
        if((__size == 0) || (__n == 0))return 0;
        int fp = (int)__stream;
        ssize_t bytes_read = read(fp, __ptr, __size * __n);
        if(bytes_read < 0) return -1;
        return bytes_read / __size;
    }
	size_t smf_posix_to_cmsis_write(const void* ptr, size_t __size, size_t __n, void* __stream){
        if((__size == 0) || (__n == 0))return 0;
        int fp = (int)__stream;
        ssize_t bytes_written = write(fp, ptr, __size * __n);
        if(bytes_written < 0) return -1;
        return bytes_written / __size;
    }
	int smf_posix_to_cmsis_seek(void* __stream, long int __off, int __whence){
        int fp = (int)__stream;
        off_t ret = lseek(fp, (off_t)__off, __whence);
        return (ret == (off_t)-1) ? -1 : 0;
    }
	long smf_posix_to_cmsis_tell(void* __stream){
        int fp = (int)__stream;
        off_t ret = lseek(fp, (off_t)0, SEEK_CUR);
        if (ret == (off_t)-1) {
            return -1;
        }
        return (long)ret;
    }
	long smf_posix_to_cmsis_length(void* __stream){
        int fp = (int)__stream;
        struct stat st;
        if (fstat(fp, &st) < 0) {
            return -1;
        }
        return (int32_t)st.st_size;
    }

    void smf_project_fs_callback() {
        smf_fs_callback_register(
            (CbFOpen)&smf_posix_to_cmsis_open,
            (CbFClose)&smf_posix_to_cmsis_close,
            (CbFRead)&smf_posix_to_cmsis_read,
            (CbFWrite)&smf_posix_to_cmsis_write,
            (CbFSeek)&smf_posix_to_cmsis_seek,
            (CbFTell)&smf_posix_to_cmsis_tell,
            (CbFLength)&smf_posix_to_cmsis_length
        );
    }
}

using namespace smf;
EXTERNC void smf_nxfs_register() {
    smf_fs_callback_register(
        (CbFOpen)&smf_posix_to_cmsis_open,
        (CbFClose)&smf_posix_to_cmsis_close,
        (CbFRead)&smf_posix_to_cmsis_read,
        (CbFWrite)&smf_posix_to_cmsis_write,
        (CbFSeek)&smf_posix_to_cmsis_seek,
        (CbFTell)&smf_posix_to_cmsis_tell,
        (CbFLength)&smf_posix_to_cmsis_length
    );
}
