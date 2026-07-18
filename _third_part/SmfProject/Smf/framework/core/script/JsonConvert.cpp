#include "JsonConvert.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;
JsonConvert::JsonConvert() {
	JsonParser::_cbpriv = this;
	JsonParser::_cb = [](void* priv, Node* node, eCmd cmd) {
		((JsonConvert*)priv)->Callback(node, cmd);
	};
}
JsonConvert::~JsonConvert() {
	dbgTestPL();
}
const char* JsonConvert::Convert(const char* str) {
	_buff = str;
	_begin = _buff.c_str();
	_ptr = _begin;
	_end = _begin + strlen(_begin);
	returnIfErrC(0,!JsonParser::Parse(str));
	return _buff.c_str();
}

void JsonConvert::Callback(Node* node, eCmd cmd) {
	switch (cmd) {
	case eCmd::init: cbInit(node); break;
	case eCmd::deinit: cbDeinit(node); break;
	case eCmd::set: cbSet(node); break;
	default:return;
	}
}

void JsonConvert::cbInit(Node* node) {
	if (node->_parent) {
		switch (Hash(node->_keys)) {
		case Hash("children"):_ptr[-1] = '{'; break;
		default:break;
		}
	}
	else {
		_ptr += sprintf(_ptr, "{");
	}
}
void JsonConvert::cbDeinit(Node* node) {
	if (node->_parent) {
		switch (node->_type) {
		case eType::array:_ptr += sprintf(_ptr, "}"); break;
		case eType::object:_ptr[-1] = ';'; break;
		case eType::int_:break;
		case eType::bool_:break;
		case eType::float_:break;
		case eType::fraction:break;
		case eType::string:break;
		default:return;
		}
	}
	else {
		_ptr += sprintf(_ptr, "}");
	}
}
void JsonConvert::cbSet(Node* node) {
	auto keys = node->Keys();
	switch (Hash(keys)) {
	case Hash("name"): //_ptr += sprintf(_ptr, "%s,", node->_string); break;
	case Hash("type"): //_ptr += sprintf(_ptr, "%s,", node->_string); break;
	case Hash("link"): _ptr += sprintf(_ptr, "%s,", node->_string); break;
	//case Hash("children"):break;
	default:
		switch (node->_type) {
		case eType::array:break;
		case eType::object:break;
		case eType::int_:	_ptr += sprintf(_ptr, "%s=#%d,", keys, node->_int); break;
		case eType::bool_:	_ptr += sprintf(_ptr, "%s=#%d,", keys, node->_bool?1:0); break;
		case eType::float_:	_ptr += sprintf(_ptr, "%s=#%f,", keys, node->_float); break;
		//case eType::fraction: _ptr += sprintf(_ptr, "%s=#%d/%d,", keys, node->_fraction.num, node->_fraction.num); break;
		case eType::string:	_ptr += sprintf(_ptr, "%s=[%s],", keys, node->_string); break;
		default:return;
		}
	}
}
