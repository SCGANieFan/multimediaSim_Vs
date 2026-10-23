/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _MTP_FS_API_H_
#define _MTP_FS_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"
#include "plat_types.h"


#define MTP_DT_REG               0x01
#define MTP_DT_DIR               0x02

int32_t  mtp_fs_closedir(void *dir);
void    *mtp_fs_opendir(const char *srcdir);
int32_t  mtp_fs_readdir(void *pdir, char *name, int32_t nlen, uint32_t *type);
int32_t  mtp_fs_stat(const char *srcdir, uint64_t *modified, uint32_t *size);
int32_t  mtp_fs_mkdir(const char *srcdir);
int32_t  mtp_fs_remove(const char *path);
int32_t  mtp_fs_rename(const char *oldpath, const char *newpath);
int32_t  mtp_fs_write(const char *path, const char *buff, uint32_t offset, uint32_t len);
int32_t  mtp_fs_read(const char *path, char *buff, uint32_t offset, uint32_t len);
int32_t  mtp_fs_statfs(const char *srcdir, uint64_t *max_capacity, uint64_t *free);
bool     mtp_fs_file_exist(const char *srcdir);
int32_t  mtp_fs_fwrite(void *fp, const char *buff, uint32_t offset, uint32_t len);
int32_t  mtp_fs_fread(void *fp, char *buff, uint32_t offset, uint32_t len);
void    *mtp_fs_fopen(const char *path);
void     mtp_fs_fclose(void *fp);

#ifdef __cplusplus
}
#endif

#endif
