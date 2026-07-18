#include "JsonParser.h"
#include "SmfFCC.h"
#include "smf_debug.h"
#include <ctype.h>
using namespace smf;

JsonParser::JsonParser(){
	_cb = [](void* priv, Node* node, eCmd cmd) {((JsonParser*)priv)->Callback(node, cmd); };
}
JsonParser::~JsonParser(){

}
void JsonParser::Register(CbUpdate cb, void* priv) {
	_cb = cb;
	_cbpriv = priv;
}
bool JsonParser::Parse(const char* str, int len, bool nocopy) {
	//dbgTestPXL(str);
	if (!len) {
		len = strlen(_ptr);
	}
	if (nocopy) {
		_begin = (char*)str;
	}
	else {
		_string.reset(len + 1);
		returnIfErrC(false, !_string);
		_begin = (char*)_string.get();
		memcpy(_begin, str, len + 1);
	}
	_ptr = _begin;
	_end = _begin + len;
	*_end = 0;
	//dbgTestPXL("%c,%p,%u", *_ptr, _ptr, len);
	SkipSpaces();
	//dbgTestPXL("%c,%p,%u", *_ptr, _ptr, len);
	eType type = TypeDetect();
	//dbgTestPXL("%c,%p,%u", *_ptr, _ptr, len);
	returnIfErrC(false, type != eType::array && type != eType::object);
	return ParseValue(0,0);
}

char* JsonParser::ParseString() {
	auto chr = *_ptr++;
	returnIfErrC(0, '\"' != chr);
	auto ptr = _ptr;
	while (*_ptr) {
		chr = *_ptr;
		if ('\"' == chr) break;
		else if ('\\' == chr) _ptr++;
		_ptr++;
	}
	*_ptr++ = 0;
	return ptr;
}

bool JsonParser::ParseValue(Node* parent, const char* keys) {
    SkipSpaces();
    eType type = TypeDetect();
	Node node{this,parent,type,keys};
	NodeUpdate(node, eCmd::init);
	bool rst = false;
	switch (type) {
	case eType::array:rst = ParseArray(node); break;
	case eType::object:rst = ParseObject(node); break;
	case eType::int_:rst = ParseInt64(node); break;
	case eType::bool_:rst = ParseBool(node); break;
	case eType::float_:rst = ParseDouble(node); break;
	case eType::fraction:rst = ParseFraction(node); break;
	case eType::string:rst = ParseString(node); break;
	default:break;
	}   
	NodeUpdate(node, eCmd::deinit);
	return rst;
}

bool JsonParser::ParseInt64(Node& node) {
   	char* end_char = 0;
   	int64_t val = strtoll(_ptr,&end_char,0);
	//node.Set(&val);
	node._int = val;
	NodeUpdate(node, eCmd::set);
	_ptr = end_char;
   	return true;
}

bool JsonParser::ParseDouble(Node& node) {
	char* end_char = 0;
	double val = strtod(_ptr, &end_char);
	//node.Set(&val);
	node._float = (float)val;
	NodeUpdate(node, eCmd::set);
	_ptr = end_char;
	return true;
}

bool JsonParser::ParseFraction(Node& node) {
	/*smf_fraction_t val{0,1};
	char *end_char = 0;
	val.den = strtoll(_ptr, &end_char, 10);
	_ptr = end_char;
	SkipSpaces();
	if (*_ptr == '/') {
		SkipSpaces();
		val.num = strtoll(_ptr, &end_char, 10);
		_ptr = end_char;
	}
	//node.Set(&val);
	NodeUpdate(node, eCmd::set);*/
	return true;
}

bool JsonParser::ParseString(Node&node) {
	auto val = ParseString();
	returnIfErrC(false,!val);
	//node.Set(&val);
	node._string = val;
	NodeUpdate(node, eCmd::set);
    return true;
}

bool JsonParser::ParseBool(Node&node) {
	auto key = fcc32low(_ptr);
	auto val = false;
	switch (key) {
	case fcc32("true"):val = true; _ptr += 4; break;
	case fcc32("false"):val = false; _ptr += 5; break;
	default:dbgErrPL(); return false;
	}
	//node.Set(&val);
	node._bool = val;
	NodeUpdate(node, eCmd::set);
	return true;
}

bool JsonParser::ParseObject(Node& node) {
	auto chr = *_ptr++;
	returnIfErrCS(false, chr != '{', "%s", node.Keys());
	SkipSpaces();
	chr = *_ptr;
	if ('}' != chr) {
		do {
			SkipSpaces();
			auto key = ParseString();
			returnIfErrCS(false, !key, "%s", node.Keys());
			SkipSpaces();
			returnIfErrCS(false, *_ptr++ != ':', "%s", node.Keys());
			SkipSpaces();
			returnIfErrCS(false, !ParseValue(&node, key), "%s", node.Keys());
			//
			SkipSpaces();
			chr = *_ptr++;
		} while (chr == ',');
	}
	else {
		_ptr++;
	}
	//dbgTestPXL(node.Keys());
	//dbgTestPXL(_ptr);
	returnIfErrCS(false, chr != '}', "[%c]%s,%s", chr, node.Keys(), node._keys ? node._keys : "");
	return true;
}

bool JsonParser::ParseArray(Node& node) {
	auto chr = *_ptr++;
	returnIfErrCS(false, chr != '[', "%s", node.Keys());
	SkipSpaces();
	chr = *_ptr;
	if (']' != chr) {
		do {
			SkipSpaces();
			returnIfErrCS(false, !ParseValue(&node), "%s", node.Keys());
			//
			SkipSpaces();
			chr = *_ptr++;
		} while (chr == ',');
	}
	else {
		_ptr++;
	}
	//dbgTestPXL(node.Keys());
	//dbgTestPXL(_ptr);
	returnIfErrCS(false, chr != ']', "[%c]%s,%s", chr, node.Keys(), node._keys ? node._keys : "");
    return true;
}

JsonParser::eType JsonParser::TypeDetect() {
    eType  type = eType::null;
	const eType infoAr = eType::array;
    const eType infoOb = eType::object;
	const eType infoNu = eType::null;
    const eType infoBo = eType::bool_;
    const eType infoIn = eType::int_;
    const eType infoDo = eType::float_;
    const eType infoFr = eType::fraction;
    const eType infoSt = eType::string;
	auto chr = *_ptr++;
    switch(chr){
		case '\"':{type = infoSt;break;}
		case '{' :{type = infoOb;break;}
		case '[' :{type = infoAr;break;}
		case 't':case 'f':{type = infoBo;break;}
		case 'n':{type=infoNu;break;}
		case '-':case '0':case '1':case '2':case '3':
		case '4':case '5':case '6':case '7':case '8':
		case '9':
		{
		    bool dot = false;
		    bool eE = false;
		    bool sprit = false;
		    bool hex = false;
		    
		    for(auto ptr = _ptr;((ptr < _end)&&(*ptr!=',')&&(*ptr !=']')&&(*ptr !='}'));ptr++)
		    {
				if ('.' == *ptr)
				{
				    dot = true;
				    break;
				}
				if('e' == *ptr || 'E' == *ptr)
				{
				    eE = true;
				    break;
				}
				if('/' == *ptr)
				{
				    sprit = true;
				    break;
				}
				if('x' == *ptr || 'X' == *ptr)
				{
					hex = true;
					break;
				}

		    }
		    if(eE || dot)
		    {
				type = infoDo;
		    }
		    else if(sprit)
		    {
				type = infoFr;
		    }
		    else if(hex){

				type = infoIn;
		    }
		    else
		    {
				type = infoIn;
		    }
		    break;
		}
		default:
			dbgErrPL();
			break;
    }
	_ptr--;
    return type;
}

void JsonParser::SkipSpaces() {
	auto ptr = _ptr;
	while (1) {
		while (' ' == *ptr || '\t' == *ptr || '\n' == *ptr || '\r' == *ptr)ptr++;
		//skip //
		if (ptr[0] == '/' && ptr[1] == '/') {
			while (*ptr && *ptr != '\n')ptr++;
			if (*ptr)
				continue;
		}
		//skip /* */
		if (ptr[0] == '/' && ptr[1] == '*') {
			ptr += 2;
			while (*ptr && !(ptr[-1] == '/' && ptr[-2] == '*'))ptr++;
			if (*ptr)
				continue;
		}
		//
		break;
	}
	_ptr = ptr;
}
///
void JsonParser::NodeUpdate(Node& node, eCmd cmd) {
	if (cmd == eCmd::set && node._parent && node._parent->_type == eType::array)
		cmd = eCmd::add;
	if (_cb)_cb(_cbpriv, &node, cmd);
}
void JsonParser::Callback(Node* node, eCmd cmd) {
	auto lvl = node->Level();
	auto path = "";// _path.c_str();
	auto keys = node->Keys();
	//node->Path(path);
	switch (cmd) {
	case eCmd::init:
		switch (node->_type) {
		case eType::array:		dbgTestPXL("init[%d]%s,%s,array", lvl, path, keys); break;
		case eType::object: 	dbgTestPXL("init[%d]%s,%s,object", lvl, path, keys); break;
		default:break;
		}
		break;
	case eCmd::deinit:
		switch (node->_type) {
		case eType::array:		dbgTestPXL("dein[%d]%s,%s,array", lvl, path, keys); break;
		case eType::object: 	dbgTestPXL("dein[%d]%s,%s,object", lvl, path, keys); break;
		default:break;
		}
		break;
	case eCmd::set:
		switch (node->_type) {
		case eType::int_:dbgTestPXL("set [%d]%s,%s:%d", lvl, path, keys, node->_int); break;
		case eType::bool_:dbgTestPXL("set [%d]%s,%s:%s", lvl, path, keys, node->_bool ? "true" : "false"); break;
		case eType::float_:dbgTestPXL("set [%d]%s,%s:%f", lvl, path, keys, node->_float); break;
		//case eType::fraction:dbgTestPXL("set [%d]%s,%s%d/%d", lvl, path, node->_fraction.num, node->_fraction.den); break;
		case eType::string:dbgTestPXL("set [%d]%s,%s:%s", lvl, path, keys, node->_string); break;
		//case eType::object:dbgTestPXL("[%d]%s", lvl, path); break;
		//case eType::array:dbgTestPXL("[%d]%s", lvl, path); break;
		default:break;
		}
		break;
	case eCmd::add:
		switch (node->_type) {
		case eType::int_:dbgTestPXL("add [%d]%s,%s:%d", lvl, path, keys, node->_int); break;
		case eType::bool_:dbgTestPXL("add [%d]%s,%s:%s", lvl, path, keys, node->_bool ? "true" : "false"); break;
		case eType::float_:dbgTestPXL("add [%d]%s,%s:%f", lvl, path, keys, node->_float); break;
			//case eType::fraction:dbgTestPXL("add [%d]%s,%s%d/%d", lvl, path, node->_fraction.num, node->_fraction.den); break;
		case eType::string:dbgTestPXL("add [%d]%s,%s:%s", lvl, path, keys, node->_string); break;
			//case eType::object:dbgTestPXL("[%d]%s", lvl, path); break;
			//case eType::array:dbgTestPXL("[%d]%s", lvl, path); break;
		default:break;
		}
		break;
	default:
		return;
	}
}
//
JsonParser::Node::Node(JsonParser* parser, Node* parent, eType type, const char* keys)
	//: _parser(parser)
	: _parent(parent)
	, _type(type)
	, _keys(keys)
{
	//_parser->NodeUpdate(this, eCmd::init);
}

JsonParser::Node::~Node() {
	//_parser->NodeUpdate(this, eCmd::deinit);
}

void JsonParser::Node::Set(void* pval) {
	switch (_type) {
	case eType::int_:_int = *(int*)pval; break;
	case eType::bool_:_bool = *(bool*)pval; break;
	case eType::float_:_float = *(float*)pval; break;
	//case eType::fraction:_fraction = *(smf_fraction_t*)pval; break;
	case eType::string:_string = *(const char**)pval; break;
	case eType::array:break;//_node = *(Node**)pval; break;
	case eType::object:break;//_node = *(Node**)pval; break;
	default:return;
	}
	//if (_parent && _parent->_type == eType::array)
	//	_parser->NodeUpdate(this, eCmd::add);
	//else
	//	_parser->NodeUpdate(this, eCmd::set);
}
const char* JsonParser::Node::Keys()const {
	return _keys ? _keys : (_parent ? _parent->Keys() : 0);
}
void* JsonParser::Node::Object()const {
	return _type==eType::object ? _object : (_parent ? _parent->Object() : 0);
}
void* JsonParser::Node::ObjectParent()const {
	return _parent ? (_type == eType::object ? _parent->Object() : _parent->ObjectParent()) : 0;
}
void JsonParser::Node::Object(void*obj) {
	if (_type == eType::object)
		_object = obj;
	else if(_parent)
		_parent->Object(obj);
}
int JsonParser::Node::Level() const {
	return _parent ? (_parent->Level() + (_parent->_type == eType::object || _parent->_type == eType::array ? 1 : 0)) : 0;
}
void JsonParser::Node::Path(char* buff) const {
	buff[0] = 0;
	rePath(buff);
}
void JsonParser::Node::rePath(char*& buff) const{
	if (_parent) {
		_parent->rePath(buff);
		if(_keys)
			buff += sprintf(buff, "/%s", _keys);
	}
}
