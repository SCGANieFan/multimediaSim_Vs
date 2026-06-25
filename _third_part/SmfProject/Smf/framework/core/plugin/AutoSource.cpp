#include "AutoSource.h"
#include "smf_debug.h"
#include "Pipeline.h"
#include "SmfHash.h"
#include "smf_api.h"
#include "URL.h"
using namespace smf;

EXTERNC void smf_source_register() {
	Object::Register<AutoSource>("src");
}
EXTERNC void smf_demuxer_register() {
	Object::Register<AutoSource>("dem");
}

bool AutoSource::open(void*) {
	auto keys = _ele._keys;
	if (!keys) {
		returnIfErrC(false, !_url);
		//dbgTestPXL("%s", _url.c_str());
		URL url(_url.c_str());
		if(fcc64(url._type) != fcc64("file")) {
			keys = fcc64(url._ext);
		}else if(!memcmp(_url.c_str(), "file-pos", strlen("file-pos"))){
			keys = fcc64(url._ext);
		}else{
			const char* fmt = smf_recognize_demuxer(_url);
			returnIfErrCS(false, !fmt, "file fmt error");
			returnIfErrCS(false, !strcmp(fmt, "open"), "open file fail");
			returnIfErrCS(false, !strcmp(fmt, "read"), "read file fail");
			//dbgTestPSL(fmt);
			keys = fcc64(fmt);
		}
		dbgTestPXL("%s,%s", &keys, _url.c_str());
	}
	return _ele.open(keys);
}
bool AutoSource::set(uint32_t key, void* val){
	switch (key) {
	case Hash("url"): _url = (char*)val; val = _url.c_str(); break;
	case Hash("select"):
	case Hash("selectParam"): return Source::set(key, val);
	}
	switch (_ele.set(key, val)) {
	case Result::True:return true;
	case Result::False:return false;
	default:return Source::set(key,val);
	}
}

bool AutoSource::get(uint32_t key, void* val) const{
	switch (_ele.get(key, val)) {
	case Result::True:return true;
	case Result::False:return false;
	default:return Source::get(key, val);
	}
}

