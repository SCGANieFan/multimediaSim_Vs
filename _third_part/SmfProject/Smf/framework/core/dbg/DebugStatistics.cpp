#include "SmfStatistics.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include <string.h>
using namespace smf;
#define GetMs() get_ms()
////
//static unsigned _dbg_tp0;
static int _dbg_dura_idx = 0;
typedef struct {
    const char* func;
    const char* file;
    const char* other;
    unsigned line;
    unsigned dura;
    unsigned cnt;
    unsigned pt0;
}dbg_dura_t;
static dbg_dura_t _dbg_duras[128];
EXTERNC void smf_statistics_reset() {
    _dbg_dura_idx = 0;
    memset(_dbg_duras, 0, sizeof(_dbg_duras));
}
EXTERNC void smf_statistics_clear() {
    for (int i = 0; i <= _dbg_dura_idx; i++) {
        dbg_dura_t* item = &_dbg_duras[i];
        item->dura = 0;
        item->cnt = 0;
        item->pt0 = 0;
    }
}
EXTERNC unsigned smf_statistics_get_time() {
    return get_ms();
}
EXTERNC int smf_statistics_get_index(const char* func, const char* file, unsigned line, const char* other) {
    /*for(int i=0;i<_dbg_dura_idx;i++){
        if(name==_dbg_duras[i].name){
            return i;
        }
    }*/
    if (_dbg_dura_idx >= 127) {
        dbgErrPXL("%s,%d", func, line);
        return 0;
    }
    int idx = ++_dbg_dura_idx;
    dbg_dura_t* item = &_dbg_duras[idx];
    item->func = func;
    item->file = file;
    item->line = line;
    item->dura = 0;
    if (!other)other = "x";
    item->other = other;
    item->cnt = 0;
    dbgTestPXL("[%d]%s,%d", idx, func, line);
    return idx;
}
EXTERNC void smf_statistics_start(int idx) {
    dbg_dura_t* item = &_dbg_duras[idx];
    item->pt0 = get_ms();
    item->cnt++;
}
EXTERNC void smf_statistics_stop(int idx) {
    dbg_dura_t* item = &_dbg_duras[idx];
    item->dura += get_ms() - item->pt0;
}

EXTERNC void smf_statistics_add(int idx, unsigned dura) {
    dbg_dura_t* item = &_dbg_duras[idx];
    item->dura += dura;
}
EXTERNC void smf_statistics_print() {
	dbgTestPDL(_dbg_dura_idx);
    for (int i = 0; i <= _dbg_dura_idx; i++) {
        dbg_dura_t& item = _dbg_duras[i];
        unsigned dura = item.dura;
        unsigned ms = dura;//TICKS_TO_MS(dura);
        const char* file = dbgPath(item.file);
		//const char* file = strrchr(item.file, '/') + 1;
        if (item.func)dbgTest("[%3d],%4u,%6u,%6ums,%s,%s(),%s/%u\n"
            , i
            , item.cnt
            , dura
            , ms
            , item.other
            , item.func
            , file
            , item.line
        );
    }
}
EXTERNC void smf_statistics_tmp(void* p) {
    //dbgTestPXL("%p",p);
}
