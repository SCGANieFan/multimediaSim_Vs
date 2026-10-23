#include "StreamConfig.h"
#include "SmfICS.stream.h"
#include "SmfFCC.h"
#include "SmfICS.h"
#include "smf_debug_ext.h"
#include "SmfLayout.h"
#include "assert.h"

using namespace smf;
using namespace smf::ssvc;

StreamConfig::StreamConfig() {
	_cfg = (cfg_t*)ics::GetStream();
	//dbgTestPXL("%p(%u,%u)(%u,%u)", this, sizeof(ics::stream_t), sizeof(cfg_t), eStreamMax, eSetidMax);
	assert(sizeof(cfg_t) <= sizeof(ics::stream_t));
}

bool StreamConfig::set(uint32_t key, void* val) {
	switch(key){
	//case Hash("cache"): _cache =(bool)val; return true;
	case Hash("streams"):return parseStream((const char*)val);
	//case Hash("hooks"):return parseHook((const char*)val);
	case Hash("sets"):return parseSet((const char*)val);
	case Hash("notifies"):return true;// parseNotify((const char*)val);
	default:return false;
	}
}
bool StreamConfig::CheckStream(uint32_t stream)const {
	return stream && (stream < eStreamMax);
}
uint64_t StreamConfig::GetName64(uint32_t stream) const {
	//returnIfErrC(0, stream >= eStreamMax);
	return _cfg->stream_name[stream];
}
const char* StreamConfig::GetName(uint32_t stream) const {
	//returnIfErrC(0, stream >= eStreamMax);
	return (const char*)&_cfg->stream_name[stream];
}
uint32_t StreamConfig::GetMsgID(uint32_t stream)const {
	//returnIfErrC(0, stream >= eStreamMax);
	return 0x80 + (_cfg->stream_item[stream].msgid << 4);
}
uint8_t StreamConfig::GetCpuID(uint32_t stream, uint8_t index)const {
	//returnIfErrC(0, stream >= eStreamMax);
	return index ? _cfg->stream_item[stream].cpu1 : _cfg->stream_item[stream].cpu0;
}
uint8_t StreamConfig::GetReEntry(uint32_t stream)const {
	//returnIfErrC(0, stream >= eStreamMax);
	return _cfg->stream_item[stream].reentry;
}
uint8_t StreamConfig::GetRefs(uint32_t stream)const {
	//returnIfErrC(0, stream >= eStreamMax);
	return _cfg->stream_item[stream].refs;
}
uint8_t StreamConfig::GetSeqNO(uint32_t stream)const {
	//returnIfErrC(0, stream >= eStreamMax);
	return _cfg->stream_item[stream].seqno;
}
void StreamConfig::SeqNOIncrease(uint32_t stream) {
	//returnIfErrC(0, stream >= eStreamMax);
	_cfg->stream_item[stream].seqno++;
}
void StreamConfig::RefsIncrease(uint32_t stream) {
	//returnIfErrC(0, stream >= eStreamMax);
	_cfg->stream_item[stream].refs++;
}
void StreamConfig::RefsReduce(uint32_t stream) {
	//returnIfErrC(0, stream >= eStreamMax);
	_cfg->stream_item[stream].refs--;
}
void StreamConfig::RefsClear(uint32_t stream) {
	//returnIfErrC(0, stream >= eStreamMax);
	_cfg->stream_item[stream].refs = 0;
}
uint32_t StreamConfig::GetStream(uint64_t name)const {
	for (uint32_t i = 0; i < eStreamMax; i++) {
		if (_cfg->stream_name[i] == name) {
			return i;
		}
	}
	return 0;
}
uint32_t StreamConfig::GetStream(const char* name)const {
	return GetStream(fcc64(name));
}
//bool StreamConfig::Hold(uint32_t stream)const {
//	Locker lck(1000);	//lock for chip
//	auto rst = _cfg->stream_item[stream].used;
//	if(!rst)_cfg->stream_item[stream].used = true;
//	return !rst;
//}
//bool StreamConfig::Release(uint32_t stream)const {
//	Locker lck(1000);	//lock for chip
//	_cfg->stream_item[stream].used = false;
//	return true;
//}
//bool StreamConfig::IsUsed(uint32_t stream)const {
//	//lock for chip
//	return _cfg->stream_item[stream].used;
//}
//uint8_t& Refs(uint32_t stream)const {
//	//returnIfErrC(0, stream >= eStreamMax);
//	return _cfg->stream_refs[stream];
//}
//uint8_t& SeqNO(uint32_t stream) const{
//	//returnIfErrC(0, stream >= eStreamMax);
//	return _cfg->stream_seqno[stream];
//}
bool StreamConfig::CheckSetID(uint32_t setid)const {
	return setid < eSetidMax;
}
const char* StreamConfig::GetSetPath(uint32_t setid)const {
	return (const char*)_cfg->set_path[setid];
}
uint8_t StreamConfig::GetSetCpuID(uint32_t setid)const {
	return _cfg->set_cpuid[setid];
}

//bool StreamConfig::CheckNotifyID(uint32_t setid)const {
//	return setid < eNotifyMax;
//}
//uint8_t StreamConfig::GetNotifyCpuID(uint32_t notify_id)const {
//	return _cfg->notify_cpuid[notify_id];
//}

bool StreamConfig::parseStream(const char*script) {
	return Layout(script, false, true, false).Parse([](void* priv, const char* keys, void* vals, char rst) {
		//dbgTestPXL("[%c]%s,%s", rst, keys, vals);
		auto& cfg = *(cfg_t*)priv;
		auto idx = strtoul(keys, 0, 0);
		returnIfWarnCS0(idx >= eStreamMax, "%u", idx);
		void* paras[8];
		Layout layout((const char*)vals, false, true, false);
		int cnt = layout.Parse(paras, ',');
		returnIfWarnCS0(cnt < 4, "%u[%u]%s", idx, cnt, vals);
		auto& item = cfg.stream_item[idx];
		item.cpu0 = strtoul((char*)paras[0], 0, 0);
		item.cpu1 = strtoul((char*)paras[1], 0, 0);
		item.msgid = (strtoul((char*)paras[2], 0, 0) - 0x80) >> 4;
		cfg.stream_name[idx] = fcc64((const char*)paras[3]);
		item.reentry = 0;
		if (cnt > 4 && ((char*)paras[4])[0] != '0')item.reentry |= 1u << 0;
		//if (cnt > 5 && paras[5] != '0')cfg.stream_flags[idx] |= 1u << 1;
		}, _cfg);
}
//bool StreamConfig::parseHook(const char* script) {
//	return Layout(script, false, true, false).Parse([](void* priv, const char* keys, void* vals, char rst) {
//		//dbgTestPXL("[%c]%s,%s", rst, keys, vals);
//		auto& cfg = *(stream_t*)priv;
//		auto idx = strtoul(keys, 0, 0);
//		returnIfWarnCS0(idx >= eHookMax, "%u", idx);
//		void* paras[4];
//		Layout layout((const char*)vals, false, true, false);
//		int cnt = layout.Parse(paras, ',');
//		returnIfWarnCS0(cnt != 2, "[%u]%s", cnt, vals);
//		cfg.hook_cpuid[idx] = strtoul((const char*)paras[0], 0, 0);
//		cfg.hook_path[idx] = (const char*)paras[1];
//		}, _cfg);
//}
bool StreamConfig::parseSet(const char* script) {
	return Layout(script, false, true, false).Parse([](void* priv, const char* keys, void* vals, char rst) {
		//dbgTestPXL("[%c]%s,%s", rst, keys, vals);
		auto& cfg = *(cfg_t*)priv;
		auto idx = strtoul(keys, 0, 0);
		returnIfWarnCS0(idx >= eSetidMax, "%u", idx);
		void* paras[2];
		Layout layout((const char*)vals, false, true, false);
		int cnt = layout.Parse(paras, ',');
		returnIfWarnCS0(cnt != 2, "[%u]%s", cnt, vals);
		cfg.set_cpuid[idx] = strtoul((const char*)paras[0], 0, 0);
		cfg.set_path[idx] = (uint32_t)paras[1];
		}, _cfg);
}
//bool StreamConfig::parseNotify(const char* script) {
	//return Layout(script, false, true, false).Parse([](void* priv, const char* keys, void* vals, char rst) {
	//	//dbgTestPXL("[%c]%s,%s", rst, keys, vals);
	//	auto& cfg = *(cfg_t*)priv;
	//	auto idx = strtoul(keys, 0, 0);
	//	returnIfWarnCS0(idx >= eNotifyMax, "%u", idx);
	//	void* paras[2];
	//	Layout layout((const char*)vals, false, true, false);
	//	int cnt = layout.Parse(paras, ',');
	//	returnIfWarnCS0(cnt != 1, "[%u]%s", cnt, vals);
	//	cfg.notify_cpuid[idx] = strtoul((const char*)paras[0], 0, 0);
	//	}, _cfg);
//}

ssvc::Locker::Locker(uint32_t timeout)
	:smf::Locker(((StreamConfig::cfg_t*)ics::GetStream())->lock, timeout)
{

}
