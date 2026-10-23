
#include <stdio.h>
#include <string.h>
#include "smf_debug.h"
#include "evf_api.h"
namespace smf {
    void folder_scan(const char* lpPath, int(*cb)(const char* fname), bool recursion, char skipFolderPrefix) {
        if (!lpPath)return;
        dbgTestPSL(lpPath);
        char buff[256] = { 0 };
        strcpy(buff, lpPath);
        char* fpath = buff;
        char* fpattern = 0;
        char* ptr = strrchr(fpath, '/');
        if (ptr) {
            *ptr = 0;
            fpattern = ptr + 1;
        }
        else {
            ptr = buff + strlen(buff);
        }
        evf_dir_t dir = { 0 };
        evf_file_t file = { 0 };
        int rst = evf_api_findfirst(&dir, &file, fpath, fpattern);
        if (rst)return;
        *ptr++ = '/';
        do {
            if (strlen(file.fname) == 0) {
                break;
            }
            strcpy(ptr, file.fname);
            //
            if (file.fattrib & FILE_ATTR_DIR) {
                if (file.fname[0] == '.') {
                }
                else if (file.fname[0] == skipFolderPrefix) {
                    dbgTestPXL("skip folder:%s", buff);
                }
                else if (recursion) {
                    //strcpy(ptr,file.fname);
                    strcat(buff, "/");
                    //dbgTestPXL("folder:%s", buff);
                    folder_scan(buff, cb, recursion, skipFolderPrefix);
                }
                else {
                    dbgTestPXL("non-recurse folder:%s", buff);
                }
            }
            else {
                cb(buff);
            }
        } while (evf_api_findnext(&dir, &file) == 0);
        evf_api_closedir(&dir);
    }

    void folder_create(const char* lpPath) {
        dbgTestPSL(lpPath);
        //evf_api_mkdir(lpPath);
    }
}