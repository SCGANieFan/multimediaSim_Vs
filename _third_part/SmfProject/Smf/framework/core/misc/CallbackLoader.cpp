#include "CallbackLoader.h"
#include "SmfHash.h"
#include "Reflection.h"
#include "smf_debug.h"
using namespace smf;
EXTERNC bool smf_lib_register(const char* name, smf_cb_para cb_entry, smf_cb_para cb_exit) {
	ILoader* loader = 0;
	if (Reflect()._loaders.Get(name, loader)) {
		auto cbloader = (CallbackLoader*)loader;
		cbloader->_entry = cb_entry;
		cbloader->_exit = cb_exit; 
	}
	else {
		loader = new CallbackLoader(cb_entry, cb_exit);
		Reflect()._loaders.Set(name, loader, true);
	}
	return true;
}
