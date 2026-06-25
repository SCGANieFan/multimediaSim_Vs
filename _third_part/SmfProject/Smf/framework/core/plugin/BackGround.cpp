#include "BackGround.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "SmfDef.gbl.h"
#include "smf_debug.h"

using namespace smf;
EXTERNC void smf_background_register() {
	BackGround::Register<BackGround>("bg");
}

BackGround::BackGround() {
	_id = fcc32("bg");
	Enable(true, Name());
	Global<BackGround>(eGlobal::background, this);
}

BackGround::~BackGround() {
	unique_lock<mutex> lck(_mtx);
	memset(_tasks, 0, sizeof(_tasks));
	Clear();
	Global<BackGround>(eGlobal::background, 0);
}

bool BackGround::set(uint32_t keys, void* vals) {
	switch (keys) {
	//case Hash("tsize"): if (!_temp) { _tempSize = (uint32_t)vals; _temp = Alloc(_tempSize); } return true;
	case Hash("enable"): Enable(vals, Name()); return true;
	case Hash("pause"): Pause(vals); return true;
	//case Hash("notify"): return NotifyEnable((uint32_t)vals);
	case Hash("invoke"): Invoke(((Func*)vals)[0], ((void**)vals)[1]); return true;
	case Hash("invokeDelay"):Invoke(((Func*)vals)[0], ((void**)vals)[1], ((uint32_t*)vals)[2]); return true;
	case Hash("free"):InvokeFree(vals); return true;
	case Hash("freeDelay"):InvokeFree(((void**)vals)[0], ((uint32_t*)vals)[1]); return true;
	case Hash("del"):InvokeDelete((Object*)vals); return true;
	case Hash("delDelay"):InvokeDelete(((Object**)vals)[0], ((uint32_t*)vals)[1]); return true;
	case Hash("ChildLock"):((unique_lock<mutex>*)vals)->reset(_mtx); return true;
	case Hash("bgStackSize"): _stackSize = (uint32_t)vals; return true;
	}
	return Base::set(keys, vals);
}

bool BackGround::InvokeDelete(Object* obj, uint32_t delay) {
	//dbgTestPXL("%s,%u",obj->Name(),delay);
	Add(obj);
	return TaskScheduler::InvokeDelete(obj, delay);
}

bool BackGround::Item::Invoke(uint32_t delay, uint32_t interval) {
	if (_flags.Check(IS_Loop))return true;
	_flags.Set(IS_Loop, true);
	return ((BackGround*)Parent())->Invoke([](void* priv) {
		//returnIfWarnC(true, !priv);
		auto obj = (BackGround::Item*)priv;
		obj->_index++;
		if (obj->FlagsExt().Check(ExtFlags::EF_Disable)) {
			delete obj;
			return true;
		}
		else if (obj->FlagsExt().Check(ExtFlags::EF_Pause)) {
			return false;
		}
		else if (!obj->Run(priv)) {
			delete obj;
			return true;
		}
		else {
			return false;
		}
		}, this, delay, interval, &_task);
}

bool BackGround::Item::Interval(uint32_t ms) {
	if (_task) {
		_task->interval = ms;
	}
	return _task;
}

bool BackGround::Item::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("invoke"):
	case Hash("delay"):Invoke((uint32_t)vals); return true;
	case Hash("loop"):Invoke(0,(uint32_t)vals); return true;
	case Hash("interval"): Interval((uint32_t)vals);  return true;
	case Hash("en"):
	case Hash("enable"):_flagsExt.Set(ExtFlags::EF_Disable, !vals); return true;
	case Hash("pause"):_flagsExt.Set(ExtFlags::EF_Pause, vals); return true;
	}
	return Base::set(keys, vals);
}

