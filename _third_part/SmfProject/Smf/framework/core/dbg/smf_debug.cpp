#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "LogRecord.h"
#include "SmfLog.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfDef.h"
using namespace smf;

extern "C" const char* dbgPath(const char* path) {
	/*if (!_dbg_path_root_offset) {
		dbgInitFilePathRootOffset();
	}
	return path?(path + _dbg_path_root_offset):0;*/
	//int len = strlen(path);
	if (!path)return NULL;
	const char*ptr=path;
	const char*last=path;
	while(*ptr){
		if (*ptr == '/' || *ptr == '\\')
			last = ptr + 1;
		ptr++;
	}
	return last;
}
extern "C" const char* dbgFunc(const char* func) {
	if (!func)return NULL;
	const char* ptr = func;
	const char* last = func;
	while (*ptr) {
		if (*ptr == ':')
			last = ptr + 1;
		ptr++;
	}
	return last;
}
//
//static char* _log_buff[512];
//static const uint32_t _log_ch_name_tbl[]{
//	 fcc32("err")// 0
//	,fcc32("warn")// 1
//	,fcc32("cmd")// 2
//	,fcc32("note")// 3
//	,fcc32("info")// 4
//	,fcc32("log")// 5
//	,fcc32("mem")// 6
//	,fcc32("test")// 7
//	,fcc32("io")// 8
//	,fcc32("demo")// 9
//	,fcc32("pl")// 10
//	,fcc32("a")// 11
//	,fcc32("b")// 12
//	,fcc32("c")// 13
//	,fcc32("d")// 14
//	,fcc32("e")// 15
//};
//static SmfLog _log;// _log_ch_name_tbl);
//static SmfLog _log(_log_buff, sizeof(_log_buff), 0);// _log_ch_name_tbl);
extern "C" void dbgOutput(unsigned chl, const void* data, int size) {
	return Log().Output(data, size, chl);
}
EXTERNC void dbgString(unsigned chl, const char* str) {
	int len = strlen(str);
	Log().Output(str, len, chl);
	Log().Output("\n", 1, chl);
}
EXTERNC void dbgDump(unsigned chl, const void* data, int size) {
	Log().Dump(chl,data,size);
}
extern "C" void dbgPrintf(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	Log().Printf(ap, 0, 0, 0, 0, 0, fmt);
	va_end(ap);
}
extern "C" void dbgPrintfc(unsigned chl, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	Log().Printf(ap, chl, 0, 0, 0, 0, fmt);
	va_end(ap);
}
extern "C" void dbgPrintfp(unsigned chl, const char* fmt, va_list * ap) {
	Log().Printf(*ap, chl, 0, 0, 0, 0, fmt);
}
extern "C" void dbgPrintfs(const char* name, unsigned chl, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	Log().Printf(ap, chl, name, 0, 0, 0, fmt);
	va_end(ap);
}
extern "C" void dbgPrintfx(const char* name, unsigned chl, const char* fmt, va_list* ap) {
	Log().Printf(*ap, chl, name, 0, 0, 0, fmt);
}
extern "C" void __dbgPrintfs(const char* name, unsigned n, ...) {
	va_list ap;
	va_start(ap, n);
	__dbgPrintfx(name, n, &ap);
	va_end(ap);
}
extern "C" void __dbgPrintfx(const char* name, unsigned chl, va_list* ap0) {
	va_list& ap = *ap0;
	SmfLog::param_t para;
	para.ch = chl & 0xff;
	//para.keys = va_arg(ap, const char*);
	//para.type = va_arg(ap, const char*);
	para.name = name;// va_arg(ap, const char*);
	para.file = dbgPath(va_arg(ap, char*));
	para.line = va_arg(ap, int);
	para.func = dbgFunc(va_arg(ap, const char*));
	auto flag = va_arg(ap, unsigned);//n & 0xffffff00;
	bool skip = flag & DBG_OUTPUT_SKIP;
	if (!skip) {
		bool dump = flag & DBG_OUTPUT_DUMP;
		bool output = flag & DBG_OUTPUT_OUTPUT;
		para.show_chn = flag & DBG_OUTPUT_CHN;
		para.show_end = flag & DBG_OUTPUT_END;
		para.show_thd = false;
		para.show_pos = flag & DBG_OUTPUT_POS;
		para.show_key = false;// flag& DBG_OUTPUT_KEYS;
		if (dump) {
			auto data = va_arg(ap, void*);
			auto size = va_arg(ap, int);
			auto line = va_arg(ap, int);
			Log().Dump(para, data, size, line);
		}
		else if (output) {
			auto data = va_arg(ap, void*);
			auto size = va_arg(ap, int);
			Log().Output(para, data, size);
		}
		else {
			para.fmt = va_arg(ap, const char*);
			para.ap = &ap;
			Log().Printf(para);
		}
	}
}

extern "C" void smf_log_init(void* buff, int size) {
	Log().Set(buff,size);
}
extern "C" void smf_log_redirect(void (*send)(const char* str)) {
	Log().Set(send);
}
extern "C" void smf_log_redirect1(void (*send)(const char* str, int size)) {
	Log().Set(send);
}
extern "C" void smf_log_redirect2(void (*send)(const char* str, int size, int channel)) {
	Log().Set(send);
}
extern "C" void smf_log_redirect3(void (*send)(const char* str, int size, int channel, void* priv), void* priv) {
	Log().Set(send, priv);
}
static inline uint32_t getMask(int ch, va_list& ap) {
	uint8_t x = ch;
	uint32_t msk = 0;
	while (x) {
		msk |= 1u << x;
		x = va_arg(ap, int);
	}
	return msk;
}
extern "C" void smf_log_set_channels(int ch, ...) {
	va_list ap;
	va_start(ap, ch);
	Log().ChannelMap(getMask(ch, ap));
	va_end(ap);
}
extern "C" void smf_log_enable_channels(int ch, ...) {
	va_list ap;
	va_start(ap, ch);
	Log().ChannelMap(getMask(ch, ap), true);
	va_end(ap);
}
extern "C" void smf_log_disable_channels(int ch, ...) {
	va_list ap;
	va_start(ap, ch);
	Log().ChannelMap(getMask(ch, ap), false);
	va_end(ap);
}
extern "C" uint32_t smf_log_get_channels_map() {
	return Log().ChannelMap();
}
extern "C" void smf_log_set_channels_map(uint32_t chmaps) {
	return Log().ChannelMap(chmaps);
}
extern "C" void smf_log_disable_channel(int channels) {
	Log().ChannelSet(channels, false);
}
extern "C" void smf_log_enable_channel(int channels) {
	Log().ChannelSet(channels, true);
}
extern "C" void smf_log_disable_channels_map(uint32_t chmaps) {
	Log().ChannelMap(chmaps, false);
}
extern "C" void smf_log_enable_channels_map(uint32_t chmaps) {
	Log().ChannelMap(chmaps, true);
}
extern "C" void smf_log_disable_output_channels(uint32_t channels_mask) {
	Log().ChannelMap(~channels_mask);
}
extern "C" void smf_log_disable_output_channel(int channel) {
	Log().ChannelSet(channel, false);
}
extern "C" void smf_log_enable_output_channel(int channel) {
	Log().ChannelSet(channel, true);
}
EXTERNC void smf_log_redirect_to_cache() {
	Log().Set((SmfLog::Func1)0);
}
#if 0
static LogRecord _record(512,dbg_chn_list);
//static TLogRecord<512> _record(dbg_chn_list);
extern "C" void dbgListSet(void*buff,int size) {
	_record.Set(buff, size);
}
extern "C" void dbgListAppend(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	_record.Append(fmt, ap);
	va_end(ap);
}
extern "C" void dbgListOutput() {
	_record.Output();
}
extern "C" void dbgListClear() {
	_record.Clear();
}
#endif
//
#if 0
extern "C" int printf(const char*s,...){
	putchar('.');
	return 0;
}
#endif
#if 0
#define dbg_chn_xxxx 15
#define dbg_xxxx_printf(n,...)				dbgChnEntry(n,##__VA_ARGS__)
void test_dbg_chn() {
	int a = 10;
	dbgChnPDL(xxxx, a);
	return;
}
#endif