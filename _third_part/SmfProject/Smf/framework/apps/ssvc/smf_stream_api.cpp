#include "StreamClient.h"
#include "smf_stream_api.h"
#include "SmfMsg.h"
#include "SmfDef.h"
#include "SmfDef.gbl.h"
#include "SmfHash.h"
#include "smf_api.h"
#include "smf_debug_ext.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;
static inline StreamClient* sclient() {
	return Global<StreamClient>(eGlobal::sclient);
}
EXTERNC bool smf_stream_register(smf_stream_message_callback_e id, smf_stream_message_cb cb, void* priv) {
	//dbgApiPL();
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->MessageCallback(id, cb, priv);
}

EXTERNC bool smf_stream_register_async(smf_stream_message_callback_e id, smf_stream_message_cb cb, void* priv) {
	//dbgApiPL();
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->MessageCallback(id, cb, priv, true);
}

EXTERNC uint32_t smf_stream_get_free(uint32_t begin, uint32_t end) {
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->GetFree(begin, end);
}

EXTERNC uint32_t smf_stream_get_seqno(uint32_t stream) {
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->GetSeqNO(stream);
}

EXTERNC bool smf_stream_check(uint32_t stream) {
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->GetConfig().CheckStream(stream);
}

EXTERNC bool smf_stream_start_params(uint32_t stream, const void* param, uint32_t size) {
	dbgApiPXL("%02x",stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(stream, param, size);
}

EXTERNC bool smf_stream_start_params_wait(uint32_t stream, const void* param, uint32_t size, uint32_t timeout) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(stream, param, size, timeout);
}

EXTERNC bool smf_stream_start_string(uint32_t stream, const char* param) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(stream, param);
}

EXTERNC bool smf_stream_start_string_wait(uint32_t stream, const char* param, uint32_t timeout) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(stream, param, timeout);
}

EXTERNC bool smf_stream_start_script(const char* script) {
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(script);
}

EXTERNC bool smf_stream_start(uint32_t stream) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(stream, 0, 0, 0);
}
EXTERNC bool smf_stream_start_wait(uint32_t stream, uint32_t timeout) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Start(stream, 0, 0, timeout);
}
EXTERNC bool smf_stream_stop(uint32_t stream) {
	dbgApiPXL("%08x,%02x", __builtin_return_address(0),stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Stop(stream);
}

EXTERNC bool smf_stream_pause(uint32_t stream) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Pause(stream);
}
EXTERNC bool smf_stream_resume(uint32_t stream) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Resume(stream);
}

EXTERNC bool smf_stream_push(uint32_t stream, smf_frame_t* frm) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Push(stream, frm);
}

EXTERNC bool smf_stream_pull(uint32_t stream, smf_frame_t* frm) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Pull(stream, frm);
}

EXTERNC bool smf_stream_set(uint32_t stream, uint32_t keys, void* data, uint32_t size) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Set(stream, keys, data, size);
}

EXTERNC bool smf_stream_get_async(uint32_t stream, uint32_t keys, smf_cb_data cb, void* priv) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Get(stream, keys, cb, priv);
}

EXTERNC bool smf_stream_get(uint32_t stream, uint32_t keys, void* data, uint32_t max, uint32_t timeout) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Get(stream, keys, data, max, timeout);
}

EXTERNC const char* smf_stream_get_name(uint32_t stream) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->GetConfig().GetName(stream);
}

EXTERNC uint64_t smf_stream_get_name64(uint32_t stream) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->GetConfig().GetName64(stream);
}

EXTERNC bool smf_stream_set32(uint32_t stream, uint32_t keys, uint32_t value) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Set32(stream, keys, value);
}

EXTERNC bool smf_stream_get32(uint32_t stream, uint32_t keys, uint32_t* value, uint32_t timeout) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Get32(stream, keys, *value, timeout);
}

EXTERNC bool smf_stream_set_volume(uint32_t stream, uint32_t volume) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->SetVolume(stream, volume);
}

EXTERNC bool smf_stream_set_volume_reduce(uint32_t stream, bool reduce) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->SetVolumeReduce(stream, reduce);
}

EXTERNC bool smf_stream_set_mute(uint32_t stream, bool mute) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->SetMute(stream, mute);
}

EXTERNC bool smf_stream_set_mute_force(uint32_t stream, bool force) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->SetMuteForce(stream, force);
}

EXTERNC bool smf_stream_seek(uint32_t stream, uint32_t timepoint) {
	dbgApiPXL("%02x", stream);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Seek(stream, timepoint);
}

EXTERNC bool smf_stream_store_param(const char* name, void* data, int size, uint32_t timeout) {
	dbgApiPXL("%s,%u", name, size);
	auto dst = IPool::Shared().Alloc(name, size, 64);
	returnIfErrC(false, !dst);
	memcpy(dst, data, size);
	cache_flush(dst, size);
	dbgApiPXL("%s,%p,%u", name, dst, size);
	if (timeout) {
		Invoke([](void* dst) {IPool::Shared().Free(dst); return true; }, dst, timeout);
	}
	return true;
}

EXTERNC bool smf_stream_hook_register(smf_hook_cb cb, void* priv) {
	//dbgApiPL();
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->HookCallback(cb, priv);
}
EXTERNC bool smf_stream_hook_start(uint32_t pointer) {
	dbgApiPXL("%02x", pointer);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Hook(pointer, true);
}
EXTERNC bool smf_stream_hook_stop(uint32_t pointer) {
	dbgApiPXL("%02x", pointer);
	auto _mmapi = (StreamClient*)sclient();
	returnIfErrC(0, !_mmapi);
	return ((StreamClient*)_mmapi)->Hook(pointer, false);
}

EXTERNC bool smf_stream_meta_parse(const char* url, smf_meta_info_t* pmeta, void* tempbuff, uint32_t tempsize){
	dbgApiPXL("%s,%u", url, tempsize);
	return smf_meta_parse(url, pmeta, tempbuff, tempsize);
}
