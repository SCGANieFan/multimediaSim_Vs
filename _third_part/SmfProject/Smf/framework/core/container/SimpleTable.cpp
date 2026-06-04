#include "SimpleTable.h"
#include "SmfLayout.h"

using namespace smf;

bool smf::Deserialize(SimpleTable<const char*>& this_, const char* script) {
	Layout layout(script, false, true, false);
	return layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		auto pack = (SimpleTable<const char*>*)priv;
		auto item = pack->Last();
		returnIfWarnC0(!item);
		//dbgTestPXL("%s[%c]%s", keys, rst, vals);
		if (rst != '[') {
			auto v = pack->Get((const char*)vals);
			returnIfWarnC0(!v);
			item->vals = *v;
		}
		else {
			item->vals = (const char*)vals;
		}
		item->keys = Hash(keys);
		pack->Append();
		//dbgTestPXL("%x,%s,%s", item->keys, keys, item->vals);
		}, &this_, 0);
}


bool smf::Deserialize(SimpleTable<void*>& this_, const char* script, uint32_t* params) {
	Layout layout(script, false, true, false);
	returnIfErrC(false, !layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		auto pack = (SimpleTable<void*>*)priv;
		auto item = pack->Last();
		returnIfWarnC0(!item);
		//dbgTestPXL("%s[%c]%s", keys, rst, vals);
		if (((char*)vals)[-1] != '[') {
			auto item = pack->Find((const char*)vals);
			if (item) {
				vals = item->vals;
			}
		}
		item->vals = vals;
		item->keys = Hash(keys);
		pack->Append();
		}, &this_, params));
	return true;
}
