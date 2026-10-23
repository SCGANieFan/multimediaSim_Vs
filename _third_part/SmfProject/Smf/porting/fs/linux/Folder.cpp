
#include <stdio.h>
#include "smf_debug.h"
#include "SmfPorting.h"
namespace smf {
	void folder_scan(const char* lpPath, void(*cb)(const char* fname, void* priv), bool recursion, char skipFolderPrefix, void* priv) {

	}
	void folder_scan(const char* lpPath, void(*cb)(const char* fname), bool recursion, char skipFolderPrefix) {
		folder_scan(lpPath, (folder_create_cb)cb, recursion, skipFolderPrefix, 0);
	}
	void folder_create(const char* lp) {
	}
	void file_remove(const char* path) {
	}

	bool file_exist(const char* path) {
		return true;
	}
}