#include "SmfLog.h"
#include "Object.h"
#include "SmfHash.h"
#include "SmfDef.h"
#include "SmfPorting.h"
#include "smf_debug.h"

using namespace smf;
//extern "C" const char* dbgPath(const char* path);
extern "C" const char* dbgFunc(const char* func);
#define dbgPath(x) x
SmfLog::SmfLog() {
	_channelDisable = ~dbg_chn_map_default;
}
void SmfLog::ChannelSet(int ch, bool en) {
	if(en)
		_channelDisable &= ~(1u << ch);
	else
		_channelDisable |= (1u << ch);
}
void SmfLog::ChannelMap(uint32_t mask, bool en) {
	if (en)
		_channelDisable &= ~mask;
	else
		_channelDisable |= mask;
}
void SmfLog::ChannelMap(uint32_t mask) {
	_channelDisable = ~mask;
}
uint32_t SmfLog::ChannelMap()const {
	return ~_channelDisable;
}
void SmfLog::Set(void* buff, int size) {
	_buff = (char*)buff;
	_size = size;
}
//void SmfLog::Set(const std::function<void(int ch, const char* str)>&func) {
//	_print = func;
//}
//void SmfLog::Set(const std::function<void(int ch, const void* str, int size)>& func) {
//	_output = func;
//}
void SmfLog::Set(Func4 func, void* priv) {
	_priv = priv;
	_output = func;
}
void SmfLog::Set(Func1 func) {
	_priv = (void*)func;
	_output = !func ? (Func)nullptr : [](const char* str, int size, int ch, void* priv) {((Func1)priv) (str); };
}
void SmfLog::Set(Func2 func) {
	_priv = (void*)func;
	_output = !func ? (Func)nullptr : [](const char* str, int size, int ch, void* priv) {((Func2)priv) (str, size); };
}
void SmfLog::Set(Func3 func) {
	_priv = (void*)func;
	_output = !func ? (Func)nullptr : [](const char* str, int size, int ch, void* priv) {((Func3)priv) (str, size, ch); };
}
bool SmfLog::Set(uint32_t key, void* val) {
	switch (key) {
	case Hash("logMask"):ChannelMap((uint32_t)val); return true;
	case Hash("logMaskEnable"):ChannelMap((uint32_t)val, true); return true;
	case Hash("logMaskDisable"):ChannelMap((uint32_t)val, false); return true;
	case Hash("logEnable"):ChannelSet((int)val, true); return true;
	case Hash("logDisable"):ChannelSet((int)val, false); return true;
	default: return false;
	}
}
void SmfLog::Output(const void* buff, int size, int ch) {
	unique_lock<mutex> lock(_mtx);
	if (checkChannel(ch)) {
		Print(buff, size, ch);
	}
}
void SmfLog::Print(const void* buff, int size, int ch) {
	if (_output) {
		_output((const char*)buff, size, ch, _priv);
	}
	_total += size;
}
bool SmfLog::checkChannel(int n) {
	if (n == -1) {
		return true;
	}
	if ((n < 0) || (n >= 32)) {
		return false;
	}
	if (_channelDisable & (1u << n)) {
		return false;
	}	
	return true;
}
void SmfLog::printFlags(char*& ptr, char* end, param_t& para) {
	//ptr += snprintf(ptr, end - ptr, "%u", get_timestamp());
	//ptr += snprintf(ptr, end - ptr, "%u[%s]", get_timestamp(), get_platform());
	//ptr += snprintf(ptr, end - ptr, "%u[%s]", get_ms(), get_platform());
	if (Root().Error().err & SMF_ERROR_MASK_CHIP_CPU)*ptr++ = '0';
	if (para.show_chn) {
		//if (_channelNameTbl)
		//	ptr += snprintf(ptr, end - ptr, "%u[%u][%s]", get_timestamp(), get_cpu_idx(), _channelNameTbl[para.ch]);
		//else
		ptr += snprintf(ptr, end - ptr, "%u[smf][%x][%x,%02u]", get_timestamp(), para.ch, get_cpu_idx(), get_thread_idx());
		// ptr += snprintf(ptr, end - ptr, "%u[%u]", get_ms(), get_cpu_idx());
		//ptr += snprintf(ptr, end - ptr, "%u[smf][%s]", get_timestamp(), __smf_platform);
		//ptr += snprintf(ptr, end - ptr, "%s[smf][%s]", get_rtc_short(), __smf_platform);
		//ptr += snprintf(ptr, end - ptr, "%u,%s[smf][%s]", get_timestamp(), get_rtc_short(), __smf_platform);
		if (ptr > end)ptr = end;
	}
}
#if 0
void SmfLog::printChannel(char*& ptr, char* end, param_t& para) {
	if (para.ch == -1) {
		return;
	}
	if (para.show_chn) {
		//ptr += snprintf(ptr, end - ptr, "[%x][%s]", para.ch, para.name ? para.name : "");
		//if(para.name)
		//	ptr += snprintf(ptr, end - ptr, "[%s]", para.name);
		//else
			ptr += snprintf(ptr, end - ptr, "[%x]", para.ch);
		//if (ptr > end)ptr = end;
		//if (end - ptr >= 6 && _channelNameTbl) {
		//	char* name = (char*)&_channelNameTbl[para.ch];
		//	*ptr++ = '[';
		//	*ptr++ = name[0];
		//	if (name[1])*ptr++ = name[1];
		//	if (name[2])*ptr++ = name[2];
		//	if (name[3])*ptr++ = name[3];
		//	*ptr++ = ']';
		//}
	}
}
#endif
#if 0
void SmfLog::printThread(char*& ptr, char* end, param_t& para) {
	if (para.show_thd) {
		//ptr += snprintf(ptr, end - ptr, "[%08x,%p]", Thread::GetID(), &para);
		if (ptr > end)ptr = end;
	}
}
#endif
void SmfLog::printPosition(char*& ptr, char* end, param_t& para) {
	if (para.show_pos && para.file) {
		ptr += snprintf(ptr, end - ptr, "%s/%u#%s()", dbgPath(para.file), para.line, dbgFunc(para.func));
		//ptr += snprintf(ptr, end - ptr, "[%08X]", Location32(dbgPath(para.file), para.line));
		if (ptr > end)ptr = end;
	}
}
#if 0
void SmfLog::printKeys(char*& ptr, char* end, param_t& para) {
	if (para.show_key && para.keys) {
		ptr += snprintf(ptr, end - ptr, "[%s]", para.keys);
		if (ptr > end)ptr = end;
	}
}
void SmfLog::printType(char*& ptr, char* end, param_t& para) {
	if (para.show_key && para.type) {
		ptr += snprintf(ptr, end - ptr, "[%s]", para.type);
		if (ptr > end)ptr = end;
	}
}
#endif
//void SmfLog::Printf(char* buff, int max, param_t& para) {
void SmfLog::Printf(param_t& para) {
	if (!checkChannel(para.ch))return;
	//
	unique_lock<mutex> lock(_mtx);
	char* buff = 0;
	auto max = getBuff(buff);
	if (max <= 4)return;

	char* ptr = buff;
	char* end = ptr + max - 4;
	printFlags(ptr, end, para);
	//printThread(ptr, end, para);
	//printChannel(ptr, end, para);	
	//printType(ptr, end, para);
	//printKeys(ptr, end, para);
	printPosition(ptr, end, para);
	//
	if (para.fmt) {
		ptr += vsnprintf(ptr, end-ptr, para.fmt, *para.ap);
		if (ptr > end)ptr = end;
	}
	if (para.show_end || ptr==end) {
		*ptr++ = '\n';
	}
	*ptr++ = 0;
	//
	Print(buff, ptr - buff - 1, para.ch);
}
void SmfLog::Output(param_t& para, const void* data, int size) {
	if (!checkChannel(para.ch))return;
	//
	unique_lock<mutex> lock(_mtx);
	char* buff = 0;
	auto max = getBuff(buff);
	if (max <= 4)return;

	char* ptr = buff;
	char* end = ptr + max - 4;
	printFlags(ptr, end, para);
	//printThread(ptr, end, para);
	//printChannel(ptr, end, para);	
	//printType(ptr, end, para);
	//printKeys(ptr, end, para);
	printPosition(ptr, end, para);
	//
	Print(buff, ptr - buff, para.ch);
	Print(data, size, para.ch);
	if (para.show_end) {
		Print("\n", 1, para.ch);
	}
}
void SmfLog::Dump(param_t& para, const void* data, int size, int line) {
	if (!checkChannel(para.ch))return;
	unique_lock<mutex> lock(_mtx);
	char* buff = 0;
	auto max = getBuff(buff);
	if (!max)return;
	char* ptr = buff;
	char* end = ptr + max - 4;
	printFlags(ptr, end, para);
	//printThread(ptr, end, para);
	//printChannel(ptr, end, para);	
	//printType(ptr, end, para);
	//printKeys(ptr, end, para);
	printPosition(ptr, end, para);

	ptr = Dump(ptr, end, data, size, line);
	Print(buff, ptr - buff - 1, para.ch);
}
void SmfLog::Dump(int ch, const void* data, int size, int line){
	if (!data || !size)return;
	if (!checkChannel(ch))return;
	//
	unique_lock<mutex> lock(_mtx);
	char* buff = 0;
	auto max = getBuff(buff);
	if (!max)return;
	//
	auto dst = Dump(buff, buff+max, data, size, line);
	Print(buff, dst - buff - 1, ch);
}
char* SmfLog::Dump(char* dst, char* end_, const void* data, int size, int line) {
	auto end = end_ - 4;
	dst += snprintf(dst, end - dst, "%p(%d):", data, size);
	auto ptr = (uint8_t*)data;
	auto ptr1 = (uint8_t*)data + size;
	while (ptr < ptr1 && dst < end) {
		dst += snprintf(dst, end-dst, "%02x%02x%02x%02x ", ptr[0], ptr[1], ptr[2], ptr[3]); ptr += 4;
	}
	*dst++ = '\n';
	*dst++ = 0;
	return dst;
}
//void SmfLog::Printf(param_t& para) {
//	unique_lock<mutex> lock(_mtx);
//	char* buff = 0;
//	if(auto bmax = getBuff(buff))
//		Printf(buff, bmax, para);
//}

void SmfLog::Printf(const char* pattern, ...) {
	va_list ap;
	va_start(ap, pattern);	
	Printf(ap,0,0,0,0,0,pattern);
	va_end(ap);
}
void SmfLog::Printf(int ch, const char* name, const char* pattern, ...) {
	va_list ap;
	va_start(ap, pattern);
	Printf(ap,ch,0,0,0,0, pattern);
	va_end(ap);
}
void SmfLog::Printf(int ch, const char* name, int line, const char* file, const char* func, const char* pattern, ...) {
	va_list ap;
	va_start(ap, pattern);
	Printf(ap, ch, name, line, file, func, pattern);
	va_end(ap);
}
void SmfLog::Printf(va_list& ap, int ch, const char* name, int line, const char* file, const char* func, const char* pattern) {
	param_t para;
	memset(&para, 0, sizeof(para));
	para.ch = ch;
	para.name = name;
	para.file = file;
	para.line = line;
	para.func = func;
	para.fmt = pattern;
	para.ap = &ap;
	para.show_end = true;
	para.show_chn = true;
	para.show_thd = false;
	Printf(para);	
}

char* SmfLog::Print(char* ptr, char* end) {
	ptr = snprintf(ptr, end, "[log]%u,%u", _total,_loss);
	return ptr;
}

int SmfLog::getBuff(char*& buff) {
	if (!_output) {
		if (_size <= _cache) {
			_loss ++;
			return 0;
		}
	}
	else if (_cache) {
		Print(_buff, _cache, 0);
		_cache = 0;
	}
	buff = _buff + _cache;
	return _size - _cache;
}
