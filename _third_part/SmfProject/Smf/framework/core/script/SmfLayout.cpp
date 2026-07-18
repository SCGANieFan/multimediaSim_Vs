#include "SmfLayout.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "URL.h"
#include "unique_ptr.h"
#include <ctype.h>

using namespace smf;
#define STR(s) (s?(const char*)&s:"")
static inline Object* createOne(Object* parent, char*& script, uint32_t* params, char& last);
static inline int pre_process(char* dst, const char* src);
static inline char parseVals(char*& script, char& last, uint32_t* params, void*& vals_, char except = '-');
static inline char parsePair(char*& script, char& last, uint32_t* params, const char*& keys, void*& vals);

bool smf::Configure(const char* url, bool is_cpp) { 
	return Layout(url, true, true, is_cpp).Create();
}
EXTERNC void* smf_create_object_from_string(const char* script, void* parent) {
	return Layout(script).Create((Object*)parent);
}
EXTERNC void* smf_create_object_with_params(const char* script, void* parent, uint32_t* params) {
	return Layout(script).Create((Object*)parent, params);
}
EXTERNC void* smf_create_object_from_io(const char* url, void* parent, uint32_t* params) {
	return Layout(url, true).Create((Object*)parent, params);
}

Layout::Layout(const char* script, bool is_url, bool no_copy, bool is_cpp) {
	if (is_url) {
		URL url(script);
		auto io = url.Create("rb");
		returnIfErrC0(!io);
		unique_ptr<IO> iox(io);
		auto len = io->GetSize();
		string str(len + 1, 64);
		returnIfErrC0(!str);
		returnIfErrC0(!io->Read((char*)str, len));
		((char*)str)[len] = 0;
		_buff = std::move(str);
		_script = _buff.c_str();
	}
	else if (no_copy) {
		_script = (char*)script;
		//dbgTestPPL(_script);
	}
	else {
		_buff = script;
		_script = _buff.c_str();
		//dbgTestPPL(_script);
	}
	if (is_cpp || _script[0] == '#' || Hash(_script, 10) == Hash("const char")) {
		pre_process(_script, _script);
	}
}
Object* Layout::Create(mutex& mtx, Object* parent, uint32_t* params) {
	unique_lock<mutex> lck(mtx);
	return Create(parent, params);
}
Object* Layout::Create(Object* parent, uint32_t* params) {
	returnIfErrC(0, !_script);
	auto script = _script;
	char last = 0;
	Object* obj = createOne(parent, script, params, last);
	while (obj && *script && last == ';') {
		if (!createOne(parent, script, params, last)) break;
	}
	return obj;
}
bool Layout::Parse(Object* parent, uint32_t* params) {
	return Create(parent, params);
}
bool Layout::Parse(void*& vals, uint32_t* params) {
	returnIfErrC(false, !_script);
	auto script = _script;
	char last = 0;
	return parseVals(script, last, params, vals);
}
bool Layout::Parse(const char*& keys, void*& vals, uint32_t* params) {
	returnIfErrC(false, !_script);
	auto script = _script;
	char last = 0;
	return parsePair(script, last, params, keys, vals);
}
bool Layout::Parse(CbPair cb, void* priv, uint32_t* params) {
	returnIfErrC(false, !_script);
	auto script = _script;
	char last = 0;
	do {
		const char* keys = 0;
		void* vals = 0;
		auto rst = parsePair(script, last, params, keys, vals);
		if (rst)cb(priv, keys, vals, rst);
	} while (last == ',');
	return true;
}
int Layout::Parse(void** vals, char spliter, uint32_t* params) {
	int i = 0;
	char last = 0;
	do {		
		parseVals(_script, last, params, vals[i], 0);
		i++;
	} while (last == spliter);
	return i;
}
bool Layout::Config(Object* obj, uint32_t* params) {
	return Parse([](void* priv, const char* keys, void* vals, char rst) {
		auto obj = (Object*)priv;
		obj->Set(keys, vals);
		},this,params);
}
/**
[pattern] name,types,links,key0=str0,key1=[val1],key2=#2,key3=$3,key4=&4,key5=@5{children},key6=val6;
[note]the items split with ',', and end with ';'
name: normal string
	"$idx": get string pointer from params
	"&idx": get string address from params
	".": the current object
	"..": the parent object
	"0": null
types: string array, split with '-'
	"0": null
links: string array, split with '-'
	"0": null
keyN=valN: params, keyword value pairs, split with "="
	keyN: normal string
	valN: normal string
		"[...]: special string, for content containing sensitive characters, such as '=' ',' ';' '{' '}' '[' ']'
		"#number": immediate number
		"$idx": set operator, (uint32_t)value at params[idx]
		"&idx": set operator, (uint32_t)address of params[idx]
		"@idx": get operator, (uint32_t)value at params[idx]
{...}: children, the format is the same as the pattern.
 */

static inline bool IsValidChar(char c) {
	return (c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || (c <= '9' && c >= '0') || (c == '_');
}
//static inline char Next(char*& script) {
//	while (IsValidChar(*script))script++;
//	auto last = *script;
//	if (last) {
//		*script = 0;
//		script++;
//	}
//	return last;
//}
static inline char Next(char*& script,char except = 0) {
	if (except)
		while (*script == except || IsValidChar(*script))script++;
	else
		while (IsValidChar(*script))script++;
	auto last = *script;
	if (last) {
		*script = 0;
		script++;
	}
	return last;
}
static inline char parseVals(char*& script, char& last, uint32_t* params, void*& vals_, char except) {
	auto vals = script;
	auto rst = *vals;
	if (rst == '#') {
		int32_t v = strtoll(vals + 1, 0, 0);
		script++;
		last = Next(script);
		vals_ = (void*)v;
	}
	else if (rst == '$') {
		uint32_t i = strtoul(vals + 1, 0, 0);
		returnIfErrC(0, !params);
		script++;
		last = Next(script);
		vals_ = (void*)params[i];
	}
	else if (rst == '&') {
		uint32_t i = strtoul(vals + 1, 0, 0);
		returnIfErrC(0, !params);
		script++;
		last = Next(script);
		vals_ = (void*)&params[i];
	}
	else if (rst == '@') {
		uint32_t i = strtoul(vals + 1, 0, 0);
		returnIfErrC(0, !params);
		script++;
		last = Next(script);
		vals_ = (void*)params[i];
	}
	else if (rst == '[') {
		int cnt = 1;
		vals++;
		script++;
		while (*script && cnt) {
			auto ch = *script++;
			if (ch == '[')
				cnt++;
			else if (ch == ']')
				cnt--;
		}
		if (!cnt) {
			script[-1] = 0;
		}
		last = Next(script);
		vals_ = vals;
	}
	else {
		last = Next(script, except);
		vals_ = vals;
	}
	// dbgTestPXL("%c,%c,%s,%p", rst, last, vals, vals_);
	return rst;
}
static inline bool parseName(char*& script, char& last, char*& name, uint32_t* params) {
	return parseVals(script, last, params, (void*&)name);
}
static inline bool parseType(char*& script, char& last, char*& type, uint32_t* params) {
	return parseVals(script, last, params, (void*&)type);
}
static inline bool parseLink(char*& script, char& last, char*& type, uint32_t* params) {
	return parseVals(script, last, params, (void*&)type, 0);
}
static inline char parsePair(char*& script, char& last, uint32_t* params, const char*& keys, void*& vals) {
	keys = script;
	last = Next(script);
	returnIfWarnCS(0, last != '=', "%c,%s", last, keys);
	auto rst = parseVals(script, last, params, vals);
	if (rst == '[' ){
		switch (fccall32((const char*)vals)) {
		case 0:
		case fcc32("NULL"):
		case fcc32(" "):
		case fcc32("  "):
			dbgWarnPXL("keys=%s,vals=%s", keys, vals);
			rst = 0;
			break;
		}
	}
	return rst;
}
static inline bool parseParam(char*& script, char& last, Object* obj, uint32_t* params) {
	do {
		if (*script == '{') {
			script++; 
			last = '{'; 
			break;
		}
		const char* keys = 0;
		void* vals = 0;
		auto rst = parsePair(script, last, params, keys, vals);
		//dbgTestPXL("%s,%p", keys, vals);
		if (rst == '@')obj->Get(keys, vals);
		else if (rst) obj->Set(keys, vals);
	} while (last == ',');
	return true;
}
static inline Object* createOne(Object* parent, char*& script, uint32_t* params, char& last) {
	returnIfWarnC(0, !script||!*script||!script[1]);
	//dbgTestPXL("%s", script);
	Object* obj = 0;
	last = 0;
	unique_lock<mutex> lck;
	{//create or find
		auto parent0 = parent;
		if (!parent0) {
			parent0 = &Object::Root();
		}
		else {
			parent->Set(Hash("ChildLock"), &lck);
		}
		char* name = 0;
		char* type = 0;// keys;
		returnIfErrC(0, !parseName(script, last, name, params)); //dbgTestPXL("%s,%p,%s", name, script, script);
		returnIfErrC(0, !parseType(script, last, type, params)); //dbgTestPXL("%s,%p,%s", type, script, script);
		uint64_t name64 = 0;
		if (name[0] == '_' && name[1] == 0) {
			name64 = UniqueID('_');
			name = (char*)&name64;
		}
		//dbgTestPXL("name=%s,type=%s-%s-%s-%s,%s", STR(name), STR(types[0]), STR(types[1]), STR(types[2]), STR(types[3]), parent ? parent->Name() : "");
		//dbgTestPXL("%s,%s,%s", name, type, parent0->Name());
		obj = parent0->Child(name);
		if(!obj){
			obj = Object::Reflect().Create<Object>(type);
			returnIfErrCS(0, !obj, "%s,%s",type,name);
			if (name) {
				obj->Name(name);
			}
			if (parent) {
				parent->Add(obj);
			}
		}
	}
	if (last == ',') {//links
		do {
			// auto link = script;
			char* link = 0;
			parseLink(script, last, link, params);
			// last = Next(script);
			// dbgTestPXL("%p,%p,%s", obj, parent, link);
			if (obj && parent && link && *link && *link != '0') {
				if(!obj->Set("linkFromX", link)) {
					dbgErrPXL("fail:%s/%s<-%s",link,obj->Name(),parent->Name());
				}
			}
		} while (*script && last == '-');
	}
	if (last == ',') {//params
		parseParam(script,last, obj, params);
	}
	if (last == '{') {//children
		do {
			returnIf(obj, !createOne(obj, script, params, last));
			if (last == ';' && *script == '}') {
				last = *script++;
				break;
			}
		} while (last == ';');
		last = *script++;
	}
	if (last == ',') {//params
		parseParam(script,last, obj, params);
	}
	//
	return obj;
}

static inline int pre_process(char* dst0, const char* src0) {
	auto str = src0;
	auto dst = dst0;
	char ch = 0;
	int line = 0;
	bool validline = false;
	bool first = true;
	// char* linestr = str;
	// int linesize = 0;
	while (str && (ch = *str)) {
		if (!validline && ch != '\n' && isspace(ch)) { str++; continue; }
		if (ch == '\n') {
			returnIfErrCS(0, validline, "syntax error:%d", line + 1);
			++line;
			++str;
			first = true;
			continue;
		}
		if (first) {
			first = false;
			if (ch == '"') {
				validline = true;
				// linestr = dst;
				// linesize = 0;
				str++; continue;
			}
			else {
				validline = false;
				str++; continue;
			}
		}
		if (!validline) { str++; continue; }
		if (ch == '"') {
			validline = false;
			// dbgOutput(0, linestr, linesize);
			// dbgOutput(0, "\n", 1);
			// sleep_for(10);
			str++; continue;
		}
		*dst++ = *str++;
		// ++linesize;
	}
	*dst = 0;
	return dst-dst0;
}

#if 0
#include "smf_api.h"
EXTERNC void testLayout() {
	smf_init();
	SMF_REGISTER(pipeline);
	SMF_REGISTER(std_pool);
	SMF_REGISTER(dumy_source);
	SMF_REGISTER(dumy_sink);
	SMF_REGISTER(bypass_filter);
	uint32_t params[]{0,1,2,3,4,5,6};
	char script[]{
		"pl0,pl{"
			"src,src-dumy;"
			"filter,filter-bypass,src,name=filter,tags=[tagStr],tags=[tag[Str]],tags=[tag[S[t]r]],flagsExtSet=#0x80;"
			"sink,sink-dumy,filter;"
		"},sts=#1;"
		"pl1,pl,0{"
			"src,src-dumy;"
			"filter,filter-bypass,src,name=filter,tags=[tagStr],tags=[tag[Str]],tags=[tag[S[t]r]],flagsExtSet=#0x80,flagsExtSet=$4,flagsExtSet=&4;"
			"sink,sink-dumy,filter;"
		"},sts=#1;"
	};
	auto obj = Layout(script).Create(0,params);
	obj->Print();
}
int main() {
	testLayout();
	return 0;
}
#endif
