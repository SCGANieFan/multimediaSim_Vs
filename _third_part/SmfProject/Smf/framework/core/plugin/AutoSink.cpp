#include "AutoSink.h"
#include "smf_debug.h"
#include "SmfHash.h"
using namespace smf;
EXTERNC void smf_sink_register() {
	Object::Register<AutoSink>("sink");
}
EXTERNC void smf_muxer_register() {
	Object::Register<AutoSink>("mux");
}

bool AutoSink::open(void* param) {
	auto keys = _ele._keys;
	if (!keys && _url) {
		dbgTestPSL(_url.c_str());
		const char* ext = strrchr(_url.c_str(), '.');
		if (ext) {
			ext += 1;
			keys = fcc64(ext);
		}
	}
	if (!keys) {
		auto media = iMedia();
		returnIfErrC(false, !media);
		auto codec = media->Codec();
		returnIfErrC(false, !codec);
		keys = codec;
	}
	return _ele.open(keys);
}
bool AutoSink::set(uint32_t key, void* val){
	switch (key) {
	case Hash("url"): _url = (char*)val; val = _url.c_str(); break;
	case Hash("select"):
	case Hash("selectParam"): return Sink::set(key, val);
	}
	switch (_ele.set(key, val)) {
	case Result::True:return true;
	case Result::False:return false;
	default:return Sink::set(key, val);
	}
}
