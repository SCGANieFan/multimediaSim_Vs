#include "SimpleArray.h"
#include "SmfLayout.h"
#include "smf_debug.h"

using namespace smf;

bool smf::Deserialize(SimpleArray<const char*>& array, const char* script) {
	//dbgTestPXL(script);
	Layout layout(script, false, true, false);
	returnIfErrC(false, !layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		auto pack = (SimpleArray<const char*>*)priv;
		auto idx = strtoul(keys, 0, 0);
		//dbgTestPXL("0x%02x[%c]%s", idx, rst, keys);
		returnIfWarnC0(idx >= pack->Count());
		if (rst == '#') {
			(*pack)[idx] = (*pack)[(int)vals];
			//dbgTestPXL("%02x", vals);
		}
		else {
			(*pack)[idx] = (const char*)vals;
			//dbgTestPXL("%s", (*pack)[idx]);
		}
		}, &array, 0));
	return true;
}

