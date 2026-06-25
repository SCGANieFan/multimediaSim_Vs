#include "AutoFilter.h"
#include "smf_debug.h"
#include "Pipeline.h"
#include "SmfHash.h"
using namespace smf;
EXTERNC void smf_filter_register() {
	Object::Register<AutoFilter>("filter");
}
EXTERNC void smf_decoder_register() {
	Object::Register<AutoFilter>("dec");
}
EXTERNC void smf_encoder_register() {
	Object::Register<AutoFilter>("enc");
}
bool AutoFilter::open(void* param) {
	auto keys = _ele._keys;
	if (!keys) {
		auto media = iMedia();
		returnIfErrC(false, !media);
		auto codec = media->Codec();
		returnIfErrC(false, !codec);
		keys = codec;
	}
	return _ele.open(keys);
}
bool AutoFilter::set(uint32_t key, void* val){
	switch (_ele.set(key, val)) {
	case Result::True:return true;
	case Result::False:return false;
	default:return Filter::set(key, val);
	}
}
