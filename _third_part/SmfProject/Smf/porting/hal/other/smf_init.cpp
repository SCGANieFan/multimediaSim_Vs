#include "SmfPorting.h"
#include <stdio.h>
#include "smf_api.h"
#include "smf_debug.h"
namespace smf {
    typedef void (*CBSend)(const char* str, int size);
    static void cbTrace(const char* str, int size) {
        //((char*)str)[size]=0;
        //printf("%s", str);
        for (int i = 0; i < size; i++)
            putchar(str[i]);
    }
    bool smf_initialize(bool reinit) {
        printf("[smf][test][dsp]%s/%d", __func__, __LINE__);
        smf_log_redirect1((CBSend)cbTrace);
        //SMF_REGISTER(std_pool);
        dbgTestPL();
        return true;
    }
}
