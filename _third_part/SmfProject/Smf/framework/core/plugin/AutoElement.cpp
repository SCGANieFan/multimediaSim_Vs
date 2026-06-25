#include "AutoElement.h"
#include "smf_debug.h"
#include "SmfHash.h"
#include "SmfExitCall.h"
#include "unique_ptr.h"
using namespace smf;

AutoElement::AutoElement(Element*this_) {
	_this = this_;
	memset(_items, 0, sizeof(_items));
	memset(_items_get, 0, sizeof(_items_get));
}
AutoElement::~AutoElement() {
	if (_this)
		_this->Status(EStatus::null);
	if (_enc)
		Free(_enc);
}

bool AutoElement::open(uint64_t keys) {
	uint64_t type = _this->Class()->Type();
	if(_cbBeforeReplace) {
		returnIfErrC(false, !_cbBeforeReplace(_types ? _types.c_str() : (const char*)&type, (const char*)&keys));
	}
	unique_ptr<Element> ele(
		_types ? Object::Create<Element>(_types.c_str()): Object::Create<Element>((const char*)&type, (const char*)&keys)
	);
	returnIfErrCS(false, !ele, "%s,%s",_types ? _types.c_str():(const char*)&type, (const char*)&keys);
	returnIfErrC(false, !_this->Replace(ele.get()));
	for (auto& item : _items) {
		if (item.keys) {
			if (!ele->Set(item.keys, (void*)item.vals)) {
				dbgWarnPXL("%08x,%08x", item.keys, item.vals);
			}
		}
	}
	for (auto& item : _items_get) {
		if (item.keys) {
			if (!ele->Get(item.keys, (void*)item.vals)) {
				dbgWarnPXL("%08x,%08x", item.keys, item.vals);
			}
		}
	}
	//
	auto obj = ele.get();
	ele.release();
	auto sts = _this->StatusNext();
	ExitCallx1 ex([](void* priv) {
		InvokeDelete((Object*)priv, 1);
		}, _this);
	return obj->Status(sts);
}
Result AutoElement::set(uint32_t key, void* val){
	switch (key) {
	case Hash("Audio"):
	case Hash("iAudio"):
	case Hash("Video"):
	case Hash("iVideo"): if(_this->iPort()) _this->iPort()->SetX(key, val); break;
	case Hash("oAudio"):
	case Hash("oVideo"): if(_this->oPort()) _this->oPort()->SetX(key, val); break;
	case Hash("keys"):_keys = fcc64((char*)val); return Result::True;
	case Hash("types"):_types = (char*)val; return Result::True;
	case Hash("cbReplace"): _cbBeforeReplace = (CbBeforeReplace_t)val; return Result::True;
	case Hash("props"): return (Result)_this->SetProperties((smf_keys_value_t*)val);
	case Hash("propsString"): return (Result)_this->SetProperties((const char*)val, 0);
	case Hash("name"):
	case Hash("id"):
	case Hash("linkto"): 
	case Hash("linkFromX"):
	case Hash("linkfrom"): return Result::Other;
	case Hash("enc"): 
	case Hash("encode"): return (Result)duplicate(key,val,_enc,sizeof(smf_media_audio_encode_t));
	case Hash("select"): return (Result)_this->SelectParamPack((const char*)val);
	}
	//
	returnIfErrC(Result::False, _sidx >= 32);
	auto& item = _items[_sidx++];
	item.keys = key;
	item.vals = (unsigned)val;
	return Result::True;
}
Result AutoElement::get(uint32_t key, void* val) const {
	for (auto& item : _items_get) {
		if (!item.keys) {
			item.keys = key;
			item.vals = (unsigned)val;
			return Result::True;
		}
	}
	return Result::False;
}
bool AutoElement::duplicate(uint32_t key, void* val, void*& dst, int size){
	dbgTestPDL(_sidx);
	returnIfErrC(false, _sidx >= 32);
	auto& item = _items[_sidx];
	dst = Alloc(size);
	returnIfErrC(false, !dst);
	memcpy(dst, val, size);
	item.keys = key;
	item.vals = (uint32_t)dst;
	_sidx++;
	return true;
}
