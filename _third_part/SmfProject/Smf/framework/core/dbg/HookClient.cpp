#include "HookClient.h"
#include "HookDefine.h"
#include "SmfPorting.h"
#include "SmfHash.h"
#include "SmfMsg.h"
#include "SmfICS.h"
#include "SmfDef.h"

using namespace smf;
using namespace smf::msg;

EXTERNC void smf_msg_client_hook_client_register() {
	HookClient::Register<HookClient>("mclient-hook-client");
}

HookClient::HookClient() {
	Name("hook");
	// _flags.Set(IS_ASync, true);
	uint32_t pointer = 0;
	for (auto& item : _items) {
		item._client = this;
		item._pointer = pointer++;
	}
	sprintf(_shmname, "%s_hook", get_cpu_name());
}
HookClient::~HookClient() {
	IPool::Shared(_shmIndex).Free("hook");
}
bool HookClient::init(IMsgService* svc, bool enable) {
	if (enable) {
	}
	else {

	}
	return true;
}

Result HookClient::Process(smf_msg_t* msg) {
	switch ((eHook)msg->cmd) {
	case eHook::hook:return (Result)Response(msgHook(msg), msg);
	default:return Result::Other;
	}
}

bool HookClient::msgHook(smf_msg_t* msg) {
	auto pointer = msg->svcid;
	returnIfErrCS(false, pointer >= 64, "%d", pointer);
	auto para = MsgGetBody<MsgHook_t>(msg);
	auto path = para->path;
	returnIfErrCS(false, !path, "%d", pointer);
	uint8_t start = msg->flags;
	return start ? msgStart(pointer, path) : msgStop(pointer, path);
}

bool HookClient::msgStart(uint32_t pointer, const char* path) {
	unique_lock<mutex> lck(_mtx);
	if (!(_flags64 & (1ull << pointer))) {
		auto& item = _items[pointer];
		returnIfErrC(false, !svc()->Set(path, &item));
		if (!_flags64) {
			auto tsize = ShmFifo::GetSize(_shmSize, _shmCount);
			auto shm = (ShmFifo*)IPool::Shared(_shmIndex).Alloc(_shmname, tsize, 64);
			returnIfErrC(false, !shm);
			shm->Initialize(tsize, _shmSize, _shmCount, true, true);
			shm->InitWrite(true, false, true);
			shm->InitRead(true, true);
			_shm = shm;
			ics::SetHook(shm);
		}
		_flags64 |= 1ull << pointer;
		dbgTestPXL("hook start:%08x,%08x", ((uint32_t*)&_flags64)[1], ((uint32_t*)&_flags64)[0]);
		return true;
	}
	dbgWarnPL();
	return false;
}

bool HookClient::msgStop(uint32_t pointer, const char* path) {
	unique_lock<mutex> lck(_mtx);
	if (_flags64 & (1ull << pointer)) {
		_flags64 &= ~(1ull << pointer);
		dbgTestPXL("hook stop:%08x,%08x", ((uint32_t*)&_flags64)[1], ((uint32_t*)&_flags64)[0]);
		returnIfErrC(false, !svc()->Set(path, 0));
		if (!_flags64) {
			if (_shm) {
				ics::SetHook(0);
				IPool::Shared(_shmIndex).Free(_shmname);
				_shm = 0;
			}
		}
		return true;
	}
	dbgWarnPL();
	return false;
}

bool HookClient::set(uint32_t keys, void* vals) {
	switch (keys) {
		//case Hash("cfg"):Config((const char*)vals); return true;
	case Hash("fmax"):
	case Hash("fifomax"):_shmCount = (uint32_t)vals; return true;
	case Hash("bmax"):
	case Hash("buffmax"):_shmSize = (uint32_t)vals; return true;
	case Hash("sidx"):_shmIndex = (uint32_t)vals; return true;
		//case Hash("delay"):_delay = (uint32_t)vals; return true;
	}
	return IMsgClient::set(keys, vals);
}

bool HookClient::Hook(Frame& frm) {
	unique_lock<mutex> lck(_mtx);
	return _shm ? _shm->Write(frm) : false;
}

///
bool HookClient::Item_t::Hook(Frame*& frm_) {
	if (!frm_ || !_client)return true;
	Frame frm = *frm_;
	frm.ext = _pointer;
	frm.sinfo = 0;
	_datacnt++;
	_datasize += frm.size;
	return _client->Hook(frm);
}

char* HookClient::print(char* ptr, char* end) const {
	ptr = IMsgClient::print(ptr, end);
	ptr = snprintf(ptr, end, ",%08x,%08x", ((uint32_t*)&_flags64)[1], ((uint32_t*)&_flags64)[0]);
	int i = 0;
	for (auto& item : _items) {
		if (_flags64 & (1ull << i)) {
			// ptr = snprintf(ptr, end, ",%d-%s", item._pointer, item._keys);
			ptr = snprintf(ptr, end, ",[%d](%d,%d)", item._pointer, item._datacnt, item._datasize);
		}
		++i;
	}
	return ptr;
}