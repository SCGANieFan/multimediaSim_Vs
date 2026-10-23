#include "smf_debug.h"
#include "SmfPorting.h"
#include "DirHandle.h"
#include "mtp_fs_api.h"
#include <string.h>
namespace smf {
void folder_scan(const char* lpPath, void(*cb)(const char* fname,void*priv), bool recursion, char skipFolderPrefix, void* priv) {
    dbgTestPL();
	if (!lpPath)return;
    dbgTestPSL(lpPath);
	char buff[256]={0};
	strcpy(buff,lpPath);
    char* fpath = buff;
    char* ptr = strrchr(fpath,'/');
	if(ptr){
		*ptr = 0;
	}else{
		ptr = buff+strlen(buff);
	}
    *ptr++ = '/'; // xx/xxx[/]
	DIR* dir=NULL;
	// FILE* fconfig=NULL;
    struct dirent* dir_entry = NULL;
    dir = opendir(lpPath);
    if(!dir){
        dbgErrPXL("unfound floder:%s",lpPath);
        return;

    }
    while((dir_entry = readdir(dir)) != NULL) {
        strcpy(ptr,dir_entry->d_name);
        if(dir_entry->d_type == DT_DIR){ //floder
            if(!recursion)continue;
        }
        else {
            dbgTestPSL(buff);
            if(cb)cb(buff,priv);
        }
	}
	//
	closedir(dir);

}

    void folder_scan(const char* lpPath, void(*cb)(const char* fname), bool recursion, char skipFolderPrefix) {
        folder_scan(lpPath, (folder_create_cb)cb, recursion, skipFolderPrefix, 0);
    }
    void file_remove(const char* path) {
        remove(path);
    }
    bool file_exist(const char* path) {
#if 1
        return mtp_fs_file_exist(path);
#else
        return false;
#endif
    }
}