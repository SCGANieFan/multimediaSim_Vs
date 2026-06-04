#include "Object.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "SmfLayout.h"
#include "smf_debug.h"
#include "unique_ptr.h"
#include "chip.h"
using namespace smf;
//
Object::Object() {
	if (!CpuCheck()) Root().Error().err |= SMF_ERROR_MASK_CPU;
	if (this != &Root())
		Root().Add(this);
}
Object::~Object() {
	dbgInfoPXL("%s,%p", Name(), this);
	unique_lock<mutex> lck(Mutex());
	_children.Clear();
	//if (_pool && !_flags.Check(DISABLE_DestroyPool)) {
	//	delete _pool;
	//}
	if (_parent) {
		_parent->Remove(this);
	}
	if (_class) {
		_class->Unload();
	}
	//onEvent(Hash("delete"));
}
void Object::initialize(Reflection::item_t* item) {
	_class = item;
	if (!_id) {
		_id = item->Keys();
		if (!_id)
			_id = item->Type();
	}
}
void Object::Name(const char* name) {
	_id = fcc64(name);
}
const char* Object::Name() const {
	return (const char*)&_id;
}
void Object::ID(uint64_t id) {
	_id = id;
}
uint64_t Object::ID() const {
	return _id;
}
bool Object::Equal(const char* name) const {
	return _id == fcc64(name);
}
bool Object::Equal(uint64_t id) const {
	return _id == id;
}
bool Object::set(uint32_t key, void* val) {
	switch (key) {
	//case SMF_PARAM_ID:
	case Hash("id"):ID(*(uint64_t*)val);return true;
	//case SMF_PARAM_Name:
	case Hash("name"):Name((const char*)val);return true;
	case Hash("parent"):_parent = (Object*)val;return true;
	//case Hash("funcMsg"):_funcMsg=*(std::function<bool(smf_message_t&)>*)val;return true;
	//case Hash("cbMsg"):_funcMsg = [val](smf_message_t& msg) {return ((cb_message)val)(&msg); }; return true;
	case Hash("cbMsg"): _cbMsg = (cb_message_t)val; return true;
	case Hash("cbMsgPriv"): _cbMsgPriv = val; return true;
	case Hash("cbMsg2"): _cbMsg = ((cb_message_t*)val)[0]; _cbMsgPriv = ((void**)val)[1]; return true;
	case Hash("msg"):receiveMessage(*(smf_message_t*)val);return true;
	//case Hash("cbMsg"):_cbMsg = (CbMsg)val;return true;
	//case Hash("cbMsgPriv"):_cbMsgPriv = val;return true;
	case Hash("root"):Root().Add(this); return true;
	//
	case Hash("run"): Run(val); return true;
	case Hash("add"): Add((Object*)val); return true;
	case Hash("addScript"): Add((const char*)val); return true;
	case Hash("addParams"): Add(((char**)val)[0], ((void**)val)[1]); return true;
	case Hash("remove"): Remove((const char*)val); return true;
	case Hash("delete"): Delete((const char*)val); return true;
	//case Hash("pool"):PoolSet((IPool*)val); return true;
	//case Hash("poolS"): _pool = IPool::Find((const char*)val); _flags.Set(DISABLE_DestroyPool); return true;
	case Hash("props"):
	case Hash("properties"):SetProperties((smf_keys_value_t*)val); return true;
	case Hash("setScript"):SetProperties((const char*)val); return true;
	case Hash("setParams"):SetProperties(((const char**)val)[0], ((void**)val)[1]); return true;
	case Hash("select"):
	case Hash("selectParam"):SelectParamPack((const char*)val); return true;
	case Hash("flagsSet"):_flags.Set((uint32_t)val, true); return true;
	case Hash("flagsClear"):_flags.Set((uint32_t)val, false); return true;
	case Hash("flagsExtSet"):_flagsExt.Set((uint32_t)val, true); return true;
	case Hash("flagsExtClear"):_flagsExt.Set((uint32_t)val, false); return true;
	case Hash("destroy"):InvokeDelete(this,(uint32_t)val); return true;
	case Hash("tags"):_tags=val; return true;
	case Hash("KeyChild"):
	case Hash("parentTags"):if(_parent)_parent->_tags=this; return true;
	//
	case Hash("log"):_flags.Set(IS_Log, (bool)val); return true;
	case Hash("live"):_flags.Set(IS_Live, (bool)val); return true;
	//
	case Hash("en"):
	case Hash("enable"):_flagsExt.Set(EF_Disable, !val); return true;
	case Hash("pause"):_flagsExt.Set(EF_Pause, val); return true;
	//
	case Hash("dbg1"):_flagsExt.Set(EF_Debug_1, val); return true;
	case Hash("dbg2"):_flagsExt.Set(EF_Debug_2, val); return true;
	case Hash("dbg3"):_flagsExt.Set(EF_Debug_3, val); return true;
	case Hash("dbg4"):_flagsExt.Set(EF_Debug_4, val); return true;
	case Hash("dbg5"):_flagsExt.Set(EF_Debug_5, val); return true;
	case Hash("dbg6"):_flagsExt.Set(EF_Debug_6, val); return true;
	case Hash("dbg7"):_flagsExt.Set(EF_Debug_7, val); return true;
	}
	//dbgInfoPXL("[%s]unknow:0x%08x,%p",Name(),key,val);
	return false;
}
bool Object::get(uint32_t key, void* val) const {
	switch (key) {
	//case SMF_PARAM_ID:
	case Hash("id"):*(uint64_t*)val = ID();return true;
	//case SMF_PARAM_Name:
	case Hash("name"):*(const char**)val = Name();return true;
	case Hash("pool"):*(IPool**)val = Pool(); return true;
	case Hash("flags"):*(uint32_t*)val = _flags; return true;
	case Hash("pflags"):*(uint32_t**)val = (uint32_t*)&_flags; return true;
	case Hash("flagsExt"):*(uint32_t*)val = _flagsExt; return true;
	case Hash("pflagsExt"):*(uint32_t**)val = (uint32_t*)&_flagsExt; return true;
	case Hash("tags"):*(void**)val = _tags; return true;
	case Hash("obj"):*(Object**)val = (Object*)this; return true;
	}
	return false;
}
bool Object::run(void* params) {
	return false;
}
bool Object::Set(uint32_t key, void* val) {
	return set(key, val);
}
bool Object::Get(uint32_t key, void* val) const {
	return get(key, val);
}
bool Object::Set(const char* key, void* val) {
	auto ptr = strrchr(key, '/');
	auto obj = this;
	if (ptr) {
		obj = Child(key, ptr);
		returnIfWarnCS(false, !obj, "[%s]%s=%p", Name(), key, val);
		key = ptr + 1;
	}	
	returnIfWarnCS(false, !obj->set(Hash(key), val), "[%s]%s=%p", obj->Name(), key, val);
	return true;
}
bool Object::Get(const char* key, void* val) const {
	auto ptr = strrchr(key, '/');
	auto obj = this;
	if (ptr) {
		obj = Child(key, ptr);
		returnIfWarnCS(false, !obj, "[%s]%s=%p", Name(), key, val);
		key = ptr + 1;
	}
	returnIfWarnCS(false, !obj->get(Hash(key), val), "[%s]%s=%p", Name(), key, val);
	return true;
}
void Object::Parent(Object*parent) {
	if (parent == _parent) {
		return;
	}
	if (_parent) {
		_parent->Remove(this);
	}
	if (parent) {
		parent->Add(this);
	}
	_parent = parent;
}
void Object::parentChange(Object* parent) {
	_parent = parent;
	//if (parent == _parent) {
	//	return;
	//}
	//auto parent0 = _parent;
	//_parent = parent;
	//if (parent0) {
	//	parent0->onEvent(Hash("remove"), this);
	//}
	//if (parent) {
	//	parent->onEvent(Hash("add"), this);
	//}
	//onEvent(Hash("ParentChanged"));
}
Object* Object::Child(const char* name, const char* end) const {
	auto obj = Child(fcc64x(name));
	return (*name == '/' && obj && (!end || name < end)) ? obj->Child(name + 1, end) : obj;
}
Object* Object::Child(uint64_t id) const {
	switch (id) {
	case fcc32(".") :return (Object*)this;
	case fcc32("..") :return (Object*)_parent;
	case fcc32("root"):return (Object*)&Root();
	default:return child(id);
	}
}
Object* Object::child(uint64_t id) const {
	auto pobj = _children.Find(id);
	return pobj ? *pobj : 0;
}
//IPool* Object::Pool()const {
//	//return _pool ? _pool : (_parent ? _parent->Pool() : GetBasePool());
//	return _pool ? _pool : GetBasePool();
//}
//void Object::PoolSet(IPool*pool){
//	if (_pool && (_pool != GetBasePool()) && (_pool!= pool))delete _pool;
//	_pool=pool;
//}
//IPool* Object::PoolGet() const {
//	return _pool;
//}
//void* Alloc(unsigned size) {
//	return Pool()->Alloc(size);
//}

//void* Alloc(unsigned size, int align, void*& alloc_addr) {
//	return Pool()->Alloc(size, align, alloc_addr);
//}

//void Object::Free(void*& ptr) {
//	if (ptr) {
//		Pool()->Free(ptr);
//		ptr = NULL;
//	}
//}

//
void Object::dbgErrProcess(const char* file, unsigned line, unsigned error)const {
	_flags.Set(IS_Error, true);
	ErrorProcess(_error, file, line, error);
	if (_flags.Check(IS_SyncError) && _parent) {
		_parent->_error = _error;
	}
}

void Object::CleanError()const {
	_flags.Set(IS_Error, false);
	_error.err64 = 0;
	if (_parent) {
		_parent->Flags().Set(IS_Error, false);
	}
}
bool Object::Register(cb_message_t func, void* priv) {
	_cbMsg = func;
	_cbMsgPriv = priv;
	return true;
}
bool Object::SendMessage(smf_message_t& msg) {
	return receiveMessage(msg);
}
bool Object::SendMessage(smf_message_t& msg, const char* msgid, smf_direction_e dir, uint32_t para0, uint32_t para1, uint32_t para2, uint32_t para3) {
	return SendMessage(msg, fcc64(msgid), dir, para0, para1, para2, para3);
}
bool Object::SendMessage(const char* msgid, smf_direction_e dir, uint32_t para0, uint32_t para1, uint32_t para2, uint32_t para3) {
	return SendMessage(fcc64(msgid), dir, para0, para1, para2, para3);
}
bool Object::SendMessage(smf_message_t& msg, uint64_t msgid, smf_direction_e dir, uint32_t para0, uint32_t para1, uint32_t para2, uint32_t para3) {
	memset(&msg, 0, sizeof(msg));
	msg.id = msgid;
	msg.creater = this;
	msg.direction = dir;
	msg.data[0] = para0;
	msg.data[1] = para1;
	msg.data[2] = para2;
	msg.data[3] = para3;
	return receiveMessage(msg);
}
bool Object::SendMessage(uint64_t msgid, smf_direction_e dir, uint32_t para0, uint32_t para1, uint32_t para2, uint32_t para3) {
	smf_message_t msg;
	return SendMessage(msg, msgid, dir, para0, para1, para2, para3);
}

bool Object::receiveMessage(smf_message_t&msg) {
	if (msg.is_processed && !msg.is_notify) {
		return true;
	}
	if (_cbMsg) {
		msg.priv = _cbMsgPriv;
		if (_cbMsg(&msg)) {
			msg.is_processed = true;
			return true;
		}
	}
	if (processMessage(msg)) {
		return true;
	}
	msg.laster = this;
	if (msg.direction == smf_direction_upward) {
		return _parent && _parent->receiveMessage(msg);
	}
	else if (msg.direction == smf_direction_downward) {
		if (msg.is_notify) {
			_children.Foreach([](Object* obj,void*msg) {
				obj->receiveMessage(*(smf_message_t*)msg);
				},&msg);
			return true;
		}
		else {
			return _children.Find([](Object* obj,void* msg) {
				return obj ? obj->receiveMessage(*(smf_message_t*)msg) : false;
				}, &msg);
		}
	}
	return false;
}

bool Object::processMessage(smf_message_t&msg) {
	switch (msg.id) {
	case fcc64("bcdown")://broadcast_down
		if (*(uint64_t*)msg.data[3] == _id) {
			msg.id = *(uint64_t*)msg.data[2];
			msg.direction = smf_direction_downward;
			SendMessage(msg);
			return true;
		}
	}
	return false;
}
void Object::Print() const {
	VoidPtr buff(4096);
	auto ptr = (char*)buff.get();
	auto end = ptr + 4096;
	auto str = ptr;
	ptr = Print(ptr, end);
	dbgTestOutputL(str,ptr-str);
	//dbgOutput(dbg_chn_test, str, ptr - str);
	//dbgOutput(dbg_chn_test, "\n", 1);
}
char* Object::Print(char* ptr, char* end) const { 
	return print(ptr, end);
}
char* Object::print(char* ptr, char* end) const {
	ptr += snprintf(ptr, end- ptr, "%s", Name());
	if (_flags.Check(IS_List)) {
		if (end < ptr + 5)return ptr;
		*ptr++ = '{';
		auto cptr = ptr;
		ptr += 3;
		int c = 0;
		uint32_t priv[3]{ (uint32_t)ptr,(uint32_t)end,0 };
		_children.Foreach([](Object* obj,void*priv) {
			auto& ptr = ((char**)priv)[0];
			auto end = ((char**)priv)[1];
			auto& c = ((uint32_t*)priv)[2];
			if (obj) {
				ptr = snprintf(ptr, end, ";%d#", c);
				ptr = obj->Print(ptr, end);
				c++;
			}
			},priv);
		ptr = ((char**)priv)[0];
		c = ((uint32_t*)priv)[2];
		sprintf(cptr, "%03d", c);
		cptr[3] = ':';
		ptr += snprintf(ptr, end - ptr, "}");
	}
	return ptr;
}

bool Object::SetProperties(smf_keys_value_t* pair) {
	while (pair->keys) {
		Set(pair->keys, pair->vals);
		pair++;
	}
	return true;
}

bool Object::SetProperties(const char* serial, void* params) {
	returnIfWarnC(false, !serial);
	Layout layout(serial);
	return layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		auto obj = (Object*)priv;
		obj->Set(keys, vals);
		}, (void*)this, (uint32_t*)params);
}
Object::ParamTable* Object::GetParamTable() const {
	return _parent ? _parent->GetParamTable() : 0;
}
bool Object::SelectParamPack(const char* keys, void* params) {
	return SelectParamPack(Hash(keys), params);
}
bool Object::SelectParamPack(uint32_t keys, void* params) {
	auto tbl = GetParamTable();
	returnIfErrC(false, !tbl);
	const char* script = 0;
	returnIfErrCS(false, !tbl->Get(keys, script), "obj name:%s", Name());
	returnIfErrC(false, !script);
	dbgExtPXL(1,"%x,%s",keys,script);
	return SetProperties(script, params);
}

bool Object::Add(const char* script, void* params) {
	return Deserialize(script,params,this);
}

void Object::processEvent(uint32_t evt, void* para) {
}
void Object::onEvent(uint32_t evt, void* para) {
	processEvent(evt, para);
}
void Object::onEvent(const char* evt, void* para) {
	processEvent(Hash(evt), para);
}
