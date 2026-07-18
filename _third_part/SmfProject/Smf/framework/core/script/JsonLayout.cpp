#include "JsonLayout.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;
JsonLayout::JsonLayout() {
	JsonParser::_cbpriv = this;
	JsonParser::_cb = [](void* priv, Node* node, eCmd cmd) {
		((JsonLayout*)priv)->Callback(node, cmd);
	};
}
JsonLayout::~JsonLayout() {
	//dbgTestPL();
}
Object* JsonLayout::Layout(const char* str, int len, uint32_t* params, bool nocopy) {
	_params = params;
	returnIfErrC(0,!JsonParser::Parse(str, len, nocopy));
	return _obj;
}

void JsonLayout::Callback(Node* node, eCmd cmd) {
	switch (cmd) {
	//case eCmd::init: cbInit(node); break;
	//case eCmd::deinit: cbDeinit(node); break;
	case eCmd::add:
	case eCmd::set: cbSet(node); break;
	default:return;
	}
}

void JsonLayout::cbInit(Node* node) {
	switch (node->_type) {
	case eType::array:break;
	case eType::object: _name = 0; _type = 0; break;
	case eType::int_:	
	case eType::bool_:	
	case eType::float_:	
	//case eType::fraction:
	case eType::string:break;
	default:return;
	}
}
void JsonLayout::cbDeinit(Node* node) {
}
void JsonLayout::cbSet(Node* node) {
	//dbgTestPSL(node->Keys());
	auto keys = node->Keys();
	switch (Hash(keys)) {
	case Hash("name"): _name = ParseString(node->_string); Find(node); break;
	case Hash("type"): _type = ParseString(node->_string); Create(node);  break;
	case Hash("link"):  Link(node, node->_string);  break;
	//case Hash("children"):break;
	default:
		switch (node->_type) {
		case eType::array:break;
		case eType::object:break;
		case eType::int_:	Set(node, keys, (void*)node->_int); break;
		case eType::bool_:	Set(node, keys, (void*)node->_bool); break;
		case eType::float_:	Set(node, keys, (void*)&node->_float); break;
		//case eType::fraction: Set(node, keys, &node->_fraction); break;
		case eType::string:
			returnIfErrC0(!node->_string);
			Set(node, keys, (void*)node->_string, node->_string[0]);
			break;
		default:break;
		}
	}
}

bool JsonLayout::Find(Node* node) {
	returnIfErrC(false, !_name);
	auto obj = (Object*)node->Object();
	if (obj) return true;
	//
	auto parent = (Object*)node->ObjectParent();
	if (!parent)parent = &Root();
	obj = parent->Child(_name);
	//dbgTestPXL("%s/%s,%p", parent->Name(), _name, obj);
	node->Object(obj);
	return true;
}
bool JsonLayout::Create(Node* node) {
	returnIfErrC(false, !_type);
	returnIfErrC(false, !_name);
	auto obj = (Object*)node->Object();
	if (!_obj)
		_obj = obj;
	if (obj)
		return true;
	Object* parent = (Object*)node->ObjectParent();
	if (!parent)parent = &Root();
	auto type = _type;
	auto name = _name;
	uint64_t name64 = 0;
	if (name[0] == '_' && name[1] == 0) {
		name64 = UniqueID(type);
		name = (char*)&name64;
	}
	//
	obj = Object::Reflect().Create<Object>(type);
	returnIfErrCS(false, !obj, "%s,%s", type, name);
	if (name) {
		obj->Name(name);
	}
	if (parent) {
		parent->Add(obj);
	}
	//
	//dbgTestPXL("%s/%s,%p", parent->Name(), _name, obj);
	node->Object(obj);
	if (!_obj)
		_obj = obj;
	return true;
}

bool JsonLayout::Link(Node* node, const char* dst) {
	auto obj = (Object*)node->Object();
	returnIfErrC(false, !obj);
	if (!dst || (dst[0] == '0' && dst[1] == 0))return true;
	auto ptr = dst;
	while (ptr && *ptr) {
		auto link = fcc64x(ptr);
		if (!link)
			break;
		returnIfErrC(false, !obj->Set("linkFromX", &link));
		if (!*ptr)
			break;
		returnIfErrC(false, *ptr != '-');
		ptr++;
	};
	return true;
}

bool JsonLayout::Set(Node* node, const char* key, void* val, char chr) {
	returnIfErrC(false, !node);
	auto obj = (Object*)node->Object();
	returnIfErrCS(false, !obj, "key=%s, val=%#X", key, val);
	if (_string) {
		if (chr == '>') {
			auto str = _string;
			_string = 0;
			return obj->Set(key, (void*)str);
		}
		else {
			strcat(_string, (const char*)val);
			return true;
		}
	}
	if (_params) {
		switch (chr) {
		case '$':return obj->Set(key, (void*)_params[strtol((char*)val+1,0,0)]);
		case '&':return obj->Set(key, (void*)&_params[strtol((char*)val+1,0,0)]);
		case '@':return obj->Get(key, (void*)&_params[strtol((char*)val+1,0,0)]);
		case '<':_string = (char*)val; *_string = 0; return true;
		default: return obj->Set(key, (void*)val);
		}
	}
	else {
		switch (chr) {
		case '<':_string = (char*)val; *_string = 0; return true;
		default: return obj->Set(key, (void*)val);
		}
	}
	return false;
}

const char* JsonLayout::ParseString(const char* str) {
	if (!str) return 0;
	switch (str[0]) {
	case '$':return (const char*)_params[strtol((char*)str + 1, 0, 0)];
	case '&':return (const char*)&_params[strtol((char*)str + 1, 0, 0)];
	default:return str;
	}
}
