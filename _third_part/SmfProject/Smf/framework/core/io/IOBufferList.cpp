#include "IOBufferList.h"
#include "smf_debug.h"
#include "mutex.h"
#include "smf_api_param.h"
using namespace smf;
//////////////////////////
EXTERNC void smf_io_buffer_item_register(const char* url, void* data, uint32_t size) {
	dbgTestPXL("%s,%p,%d", url, data, size);
	IOBufferList* list;
	{
		unique_lock<mutex> _lck(Mutex());
		list = (IOBufferList*)Root().Child("iolist");
		if (!list) {
			list = new IOBufferList();
			list->Name("iolist");
		}
	}
	list->_table.Set(url, { data, size });
}

EXTERNC void smf_io_buffer_list_register(const char* name) {
	Object::Register<IOBufferX>(name);
}

bool IOBufferX::Open(void* para) {
	returnIfErrC(false, !para);
	auto url = ((smf_io_param_t*)para)->url;
	dbgTestPSL(url);
	IOBufferList* list = 0;
	{
		unique_lock<mutex> _lck(Mutex());
		list = (IOBufferList*)Root().Child("iolist");
	}
	returnIfErrC(false, !list);
	auto pair = list->_table.Get(url);
	returnIfErrC(false, !pair);
	Reset(pair->_1, pair->_2);
	return IOBuffer::Open(para);
}
