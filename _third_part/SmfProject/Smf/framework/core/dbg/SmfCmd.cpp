#include "SmfCmd.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "SmfPorting.h"
#include "SmfDef.gbl.h"
#include "smf_debug.h"
#include "unique_ptr.h"
#include "thread.h"

using namespace smf;
static bool RemoteEntry(int argc, char** argv);
SmfCmd::SmfCmd(SimpleTable<CbEntry>&items):_items(items) {
	_id = fcc32("cmd");
}

bool SmfCmd::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("remote"): return Entry((cmd_t*)vals);
	case Hash("entry"): return Entry((argv_t*)vals);
	default: return Object::set(keys, vals);
	}
	return true;
}

int SmfCmd::Entry(int argc, char** argv) {
	char buff[64];
	auto ptr = buff;
	ptr = snprintf(ptr, ptr + sizeof(buff), "%u:", argc);
	for (int i = 0; i < argc; i++)
		ptr = snprintf(ptr, ptr + sizeof(buff), "%s ", argv[i]);
	dbgTestPXL(buff);
	if (argc < 2) {
		dbgErrPXL("[smf] please spcify fun to run");
		return -1;
	}
	//dbgTestPXL("%d,%s,%s",argc,argv[0],argv[1]);
	auto cmd = argv[1];
	returnIfErrC(-1, !cmd);
	CbEntry cb = 0;
	returnIfErrCS(-1, !_items.Get(cmd,cb), "cmd not found:%s",cmd);
	returnIfErrC(-1, !cb);
	cb(argc - 1, argv + 1);
	return 0;
}

bool SmfCmd::Register(const char* name, CbEntry cb) {
	unique_lock<mutex> lck(_mtx);
	returnIfErrC(false,!_items.Set(name, cb, true));
	return true;
}

static bool RemoteEntry(int argc, char** argv) {
	returnIfErrC(false, argc < 2);
	argc--;
	argv++;
	auto cpuid = strtoul(*argv, 0, 0);
	returnIfErrC(false, !cpuid);
	returnIfErrC(false, cpuid > 7);
	VoidBuffer buff(128, 4);
	auto cmd = (SmfCmd::cmd_t*)buff.get();
	cmd->argc = argc;
	int offset = 0;
	for (int i = 0; i < argc; i++) {
		cmd->offset[i] = offset;
		auto len = strlen(argv[i]) + 1;
		memcpy(cmd->buff + offset, argv[i], len);
		offset += len;
	}
	return RemoteSet(cpuid, "cmd/remote", cmd, sizeof(SmfCmd::cmd_t) + offset);
}

static bool InvokeEntry(int argc, char** argv) {
	returnIfErrC(false, argc < 2);
	SmfCmd::argv_t cmd{ argc, argv };
	auto rst = Invoke([](void* priv) {
		Root().Set("cmd/entry", priv);
		return true;
		}, &cmd);
	if(rst){
		sleep_for(1000);
	}
	return true;
}

bool smf::Register(const char* name, CbEntry callback) {
	auto cmd = Global<SmfCmd>(eGlobal::cmd);
	if (!cmd) {
		unique_lock<mutex> lck(Mutex());
		cmd = new SmfCmdC<128>();
		returnIfErrC(false, !cmd);
		Global<SmfCmd>(eGlobal::cmd, cmd);
		cmd->Register("remote", (CbEntry)RemoteEntry);
		cmd->Register("invoke", (CbEntry)InvokeEntry);
	}
	return cmd->Register(name, callback);
}

int smf::Entry(int argc, char** argv) {
	auto cmd = Global<SmfCmd>(eGlobal::cmd);
	returnIfErrC(false, !cmd);
	return cmd->Entry(argc, argv);
}

bool SmfCmd::Entry(cmd_t* cmd) {
	char* argv[8];
	for (int i = 0; i < cmd->argc; i++) {
		argv[i] = cmd->buff + cmd->offset[i];
	}
	smf::Entry(cmd->argc, argv);
	return true;
}
bool SmfCmd::Entry(argv_t* cmd) {
	smf::Entry(cmd->argc, cmd->argv);
	return true;
}