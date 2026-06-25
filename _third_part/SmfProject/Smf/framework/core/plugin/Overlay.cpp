#include "Overlay.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "SmfPorting.h"
using namespace smf;

EXTERNC void smf_overlay_register() {
	Overlay::Register<Overlay>("overlay");
}

Overlay::Overlay(){
}
Overlay::~Overlay(){
	dbgTestPL();
	overlay_unload_all();
}

bool Overlay::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("load"):return Load((const char*)val);
	case Hash("loadMedia"):return Load((IMedia*)val);
	case Hash("unload"):return UnLoad();
	}
	if (Object::set(key, val))return true;
	return Add((uint32_t)val,key);
}
bool Overlay::Add(uint32_t idx, uint32_t hash) {
	returnIfErrC(false, idx > sizeof(_tbl)/sizeof(_tbl[0]));
	_tbl[idx] = hash;
	return true;
}
bool Overlay::Find(uint32_t& idx, uint32_t hash) {
	for (int id = 0; id < sizeof(_tbl) / sizeof(_tbl[0]); id++) {
		if (hash == _tbl[id]) {
			idx = id;
			return true;
		}
	}
	return false;
}
bool Overlay::Load(uint32_t hash) {
	returnIfErrC(false, !hash);
	uint32_t id = 0;
	returnIfErrC(false, !Find(id, hash));
	dbgNotePDL(id);
	returnIfErrC(false, !overlay_load(id));	
	return true;
}
bool Overlay::Load(const char* codec) {
	returnIfErrC(false, !codec);
	returnIfErrCS(false, !Load(Hash(codec)),"%s",codec);
	return true;
}
bool Overlay::Load(IMedia* media) {
	returnIfErrC(false, !media);
	uint32_t id = 0;
	returnIfWarnCS(false, !Find(id, Hash(media->CodecString())),"%s", media->CodecString());
	dbgNotePDL(id);
	if (!overlay_load(id)) {
		dbgErrPXL("[%d]%s", id, media->CodecString());
		media->SetError();
		return false;
	}
	return true;
}
bool Overlay::UnLoad(){
	returnIfErrC(false, !overlay_unload_all());
	return true;
}
