#include "smf_debug.h"
namespace smf {
	void folder_scan(const char* lpPath, void(*cb)(const char* fname), bool recursion, char skipFolderPrefix) {
	}

	void folder_create(const char* lp) {
	}
	void file_remove(const char* path) {
		return;
	}
	bool file_exist(const char* path) {
		return false;
	}
}