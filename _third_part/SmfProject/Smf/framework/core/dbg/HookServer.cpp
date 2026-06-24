#include "HookServer.h"
#include "HookDefine.h"
#include "SmfHash.h"
#include "SmfDef.h"
#include "SmfICS.h"
#include "thread.h"
#include "SmfPorting.h"
#include "URL.h"
#include <time.h>
#include "SmfLayout.h"

using namespace smf;
using namespace smf::msg;

EXTERNC void smf_msg_client_hook_server_register() {
	HookServer::Register<HookServer>("mclient-hook-server");
}

HookServer::HookServer() {
	Name("hooksvc");
	memset(_items, 0, sizeof(_items));
	_flags.Set(IS_ASync, false);
}
HookServer:: ~HookServer() {
	_flags64 = 0;
	//sleep_for(10);
	//
	unique_lock<mutex> lck(_mtx);
	for (auto& io : _items) {
		if (io._io)
			delete io._io;
	}
}

bool HookServer::init(IMsgService* svc, bool enable) {
	if (enable) {
		MsgRouteRegister((uint16_t)eHook::hookServer);
	}
	else {

	}
	return true;
}

Result HookServer::Process(smf_msg_t* msg) {
	switch ((eHook)msg->cmd) {
	case eHook::hookServer: return (Result)Response(msgHook(msg),msg);
	case eHook::hookResponse: return Result::True;
	default:return Result::Other;
	}
}

bool HookServer::msgHook(smf_msg_t* msg) {
	auto pointer = msg->svcid;
	returnIfErrCS(false, pointer >= 64, "%d", pointer);
	uint8_t start = msg->flags;
	dbgTestPXL("%02x,%u", pointer, start);
	return start ? Start(pointer) : Stop(pointer);
}
bool HookServer::Start(uint32_t pointer) {
	unique_lock<mutex> lck(_mtx);
	returnIfErrCS(false, pointer >= 64, "%d", pointer);
	auto& item = _items[pointer];
	returnIfErrC(false, !item._path);
	dbgTestPXL("%u,%s", item._cpuid, item._path);
	if (_filePattern) {
		auto& refio = _items[pointer]._io;
		if (refio) {
			delete refio;
		}
		char fname[64];
		auto rtc = get_rtc_ms();
		auto timer = (time_t)rtc / 1000;
		auto tm = localtime(&timer);
		snprintf(fname, 64, _filePattern
			, pointer
			, tm->tm_year, tm->tm_mon, tm->tm_mday
			, tm->tm_hour, tm->tm_min, tm->tm_sec
			, rtc % 1000);

		URL url(fname);
		auto io = url.Create("wb");
		refio = io;
	}
	//
	dbgTestPXL("%llx", _flags64);
	item._datasize = 0;
	item._datacnt = 0;
	if (!_flags64) {
		Invoke([](void* priv) {
			return ((HookServer*)priv)->callback();
			}, this, _interval, _interval);
	}
	_flags64 |= 1ull << pointer;
	// dbgTestPXL("hook start:%08x,%08x", ((uint32_t*)&_flags64)[1], ((uint32_t*)&_flags64)[0]);
	JobHold();
	return Hook(pointer, true);
}
bool HookServer::Stop(uint32_t pointer) {
	unique_lock<mutex> lck(_mtx);
	returnIfErrCS(false, pointer>=64, "%d", pointer);
	_flags64 &= ~(1ull << pointer);
	//
	auto& item = _items[pointer];
	if (item._io) {
		delete item._io;
		item._io = 0;
	}
	item._datasize = 0;
	item._datacnt = 0;
	// dbgTestPXL("hook stop:%08x,%08x", ((uint32_t*)&_flags64)[1], ((uint32_t*)&_flags64)[0]);
	JobRelease();
	return Hook(pointer, false);
}
bool HookServer::Register(smf_hook_cb hook, void* priv) {
	unique_lock<mutex> lck(_mtx);
	_cbhook = hook;
	_cbpriv = priv;
	return true;
}

bool HookServer::callback() {
	//dbgTestPL();
	unique_lock<mutex> lck(_mtx);
	if(!_flags64) return true;
	bool ret = !_flags64;
	for (int i = 1; i < 8; i++) {
		auto shm = (ShmFifo*)ics::GetHook(i);
		if (shm && ics::IsReady(i)) {
			Frame frm;
			while (shm->Read(frm)) {
				uint32_t pointer = (uint32_t)frm.ext;
				auto& item = _items[pointer];
				if (_cbhook) {
					_cbhook(pointer, frm.Data(), frm.size, _cbpriv);
				}
				auto io = item._io;
				if (io) {
					// dbgTestPXL("hook(%d)[%d]%p,%d", pointer, item._datacnt, frm.Data(), frm.size);
					io->Write(frm.Data(), frm.size);
				}
				item._datacnt++;
				item._datasize += frm.size;
				frm.size = 0;
			}
		}
	}
	return ret;
}
bool HookServer::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("hookStart"):Start((uint32_t)vals); return true;
	case Hash("hookStop"):Stop((uint32_t)vals); return true;
	case Hash("hook"):Register(((Tuple2<smf_hook_cb,void*>*)vals)->_1, ((Tuple2<smf_hook_cb, void*>*)vals)->_2); return true;
	case Hash("pattern"):_filePattern = (const char*)vals; return true;
	//case Hash("fmax"):
	//case Hash("fifomax"):_shmCount = (uint32_t)vals; return true;
	//case Hash("bmax"):
	//case Hash("buffmax"):_shmSize = (uint32_t)vals; return true;
	case Hash("interval"):_interval = (uint32_t)vals; return true;
	case Hash("cfg"): return Config((const char*)vals);
	}
	return IMsgClient::set(keys, vals);
}

char* HookServer::print(char* ptr,char*end) const {
	unique_lock<mutex> lck((mutex&)_mtx);
	ptr = IMsgClient::print(ptr, end);
	ptr = snprintf(ptr, end, "{%p,%08x-%08x", _cbhook, ((uint32_t*)&_flags64)[1], ((uint32_t*)&_flags64)[0]);
	for (int i=0; i<64; ++i) {
		auto& item = _items[i];
		if (_flags64 & (1 << i)) {
			ptr = snprintf(ptr, end, ",%d(%d,%d)", i, item._datacnt, item._datasize);
		}
	}
	ptr = snprintf(ptr, end, "}");
	return ptr;
}

bool HookServer::Config(const char* script) {
	return Layout(script, false, true, false).Parse([](void* priv, const char* keys, void* vals, char rst) {
		//dbgTestPXL("[%c]%s,%s", rst, keys, vals);
		auto* cfg = (HookServer::item_t*)priv;
		auto idx = strtoul(keys, 0, 0);
		returnIfWarnCS0(idx >= 64, "%u", idx);
		void* paras[4];
		Layout layout((const char*)vals, false, true, false);
		int cnt = layout.Parse(paras, ',');
		returnIfWarnCS0(cnt != 2, "[%u]%s", cnt, vals);
		cfg[idx]._cpuid = strtoul((const char*)paras[0], 0, 0);
		cfg[idx]._path = (const char*)paras[1];
		}, _items);
}

bool HookServer::Hook(uint32_t pointer, bool start) {
	auto& item = _items[pointer];
	auto path = item._path;
	auto cpuid = item._cpuid;
	dbgTestPXL("[%u]%02x,%u,%s",start, pointer, cpuid, path);
	//
	TMsg<XMsgHook<64>> msg;
	memset(&msg, 0, sizeof(msg));
	msg.size = sizeof(msg);
	msg.cmd = (uint16_t)eHook::hook;
	msg.svcid = pointer;
	msg.flags = start ? 1 : 0;
	msg.cpudst = cpuid;
	strcpy(msg.path, path);
	return Send(&msg);
}
