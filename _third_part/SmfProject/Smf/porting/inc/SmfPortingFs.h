#pragma once
namespace smf {
    void smf_project_fs_callback();
	//
	typedef void(*folder_create_cb)(const char* fname, void* priv);
	void folder_scan(const char* path, void(*cb)(const char* fname, void* priv), bool recursion, char skipFolderPrefix, void* priv);
	void folder_create(const char* path);
	void file_remove(const char* path);
	bool file_exist(const char* path);
}