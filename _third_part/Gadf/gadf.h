#pragma once
void* gadf_get_register_info();
void gadf_run_by_info(void* info);
bool gadf_register_info(const char* key, class Gadf_c* info);
