#include "Element.h"
#include "SmfHash.h"
#include "smf_api.h"
using namespace smf;
///
EXTERNC void* smf_create_decoder(const char* keyswords) {
	return Object::Create<Object>("dec", keyswords);
}
EXTERNC bool smf_decode(void* hd, smf_frame_t* input, smf_frame_t* output) {
	return smf_process(hd, input, output);
}

EXTERNC void* smf_create_encoder(const char* keyswords) {
	return Object::Create<Object>("enc", keyswords);
}
EXTERNC bool smf_encode(void* hd, smf_frame_t* input, smf_frame_t* output) {
	return smf_process(hd, input, output);
}

EXTERNC void* smf_create_processer(const char* keyswords) {
	return Object::Create<Object>("proc", keyswords);
}
EXTERNC void* smf_create_demuxer(const char* keyswords) {
	return Object::Create<Object>("dem", keyswords);
}
EXTERNC void* smf_create_muxer(const char* keyswords) {
	return Object::Create<Object>("mux", keyswords);
}
