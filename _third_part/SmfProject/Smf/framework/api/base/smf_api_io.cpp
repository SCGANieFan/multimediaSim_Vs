#include "IO.h"
#include "smf_debug.h"
#include "smf_api_param.h"
using namespace smf;

EXTERNC FILE* iofopen(const char* filename, const char* mode) {
    auto io = IO::Create<IO>("io-file");
    returnIfErrC(0, !io);
    smf_io_param_t param{ filename,mode,0 };
    if(!io->Open(&param)){
        delete io;
        return 0;
    }
    return (FILE*)io;
}

EXTERNC size_t iofread(void* ptr, size_t size, size_t count, FILE* stream) {
    return ((IO*)stream)->Read(ptr, size * count);
}

EXTERNC size_t iofwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
    return ((IO*)stream)->Write((void*)ptr, size * count);
    // return count * size;
}

EXTERNC int iofclose(FILE* stream) {
    delete (IO*)stream;
    return 0;
}

EXTERNC int iofseek(FILE* stream, long offset, int whence) {
    return ((IO*)stream)->Seek(offset, (Position)whence);
}

EXTERNC long ioftell(FILE* stream) {
    return ((IO*)stream)->GetOffset();
}

#if 0
bool test_ipcfs() {
    dbgTestPL();
    FILE *fp = 0;
    fp = fopen("/data/hello.txt", "wb");
    returnIfErrC(false, !fp);
    const char *str = "hello world!";
    fwrite(str, 1, strlen(str), fp);
    fclose(fp);

    fp = fopen("/data/hello.txt", "rb");
    returnIfErrC(false, !fp);
    char buf[64] = {0};
    fread(buf, 1, sizeof(buf), fp);
    dbgTestPSL(buf);
    fclose(fp);
    dbgTestPL();
    return true;
}
#endif