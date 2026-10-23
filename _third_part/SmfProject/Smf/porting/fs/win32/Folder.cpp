#include <stdio.h>
#include<windows.h>
#include<iostream>
#include<string>
#include "smf_debug.h"
#include "SmfPorting.h"
#include "SmfPortingFs.h"
namespace smf {
	void folder_scan(const char* lpPath, void(*cb)(const char* fname, void* priv), bool recursion, char skipFolderPrefix, void* priv) {
		if (!lpPath)return;
		dbgTestL(lpPath);
		std::string fpath = lpPath;
		std::string path = lpPath;
		if (fpath.find_last_of('.') == std::string::npos)
			fpath += "/*.*"; //std::cout << fpath << std::endl;
		else
			path = path.substr(0, fpath.find_last_of("/"));
		WIN32_FIND_DATAA FindFileData;
		HANDLE hFind = ::FindFirstFileA(fpath.c_str(), &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind)    return;
		do {
			std::string fname = path;
			fname += "/";
			fname += FindFileData.cFileName;
			//
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (FindFileData.cFileName[0] == '.') {
				}
				else if (FindFileData.cFileName[0] == skipFolderPrefix) {
					dbgTestL("skip:%s", fname.c_str());
				}
				else if (recursion) {
					folder_scan(fname.c_str(), cb, recursion, skipFolderPrefix, priv);
				}
				else {
					dbgTestL("non-resurse:%s", fname.c_str());
				}
			}
			else {
				cb(fname.c_str(), priv);
			}
		} while (FindNextFileA(hFind, &FindFileData));
		FindClose(hFind);
	}
	void folder_scan(const char* lpPath, void(*cb)(const char* fname), bool recursion, char skipFolderPrefix) {
		folder_scan(lpPath, (folder_create_cb)cb, recursion, skipFolderPrefix, 0);
	}
	void folder_create(const char* lp) {
		CreateDirectoryA(lp, NULL);
	}
	void file_remove(const char* path) {
		DeleteFile((LPCWSTR)path);
	}

	bool file_exist(const char* path) {
		DWORD attributes = GetFileAttributesA(path);
		return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
	}
}