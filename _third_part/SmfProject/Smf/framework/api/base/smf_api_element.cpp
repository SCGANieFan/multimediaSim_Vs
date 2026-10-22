#include "Element.h"
#include "Hook.h"
#include "smf_api.h"
#include "smf_error.h"
#include "smf_debug.h"
#include "SmfPorting.h"
using namespace smf;
///
template<class T>
static T* To(void* obj,EFlags flags) {
	returnIfErrC(0, !obj);
	returnIfErrC(0, !((Object*)obj)->Flags().CheckAll(flags));
	return (T*)(Object*)obj;
}
static Port* iPort(void* obj) {
	Element* ele = To<Element>(obj, IS_Element);
	//returnIfErrC(0, !ele);
	returnIfErrC(0, !ele->iPort());
	return ele->iPort();
}
static Port* oPort(void* obj) {
	Element* ele = To<Element>(obj, IS_Element);
	//returnIfErrC(0, !ele);
	returnIfErrC(0, !ele->oPort());
	return ele->oPort();
}
///
EXTERNC bool smf_open(void* hd, void* para) {
	auto ele = To<Service>(hd, IS_Service);
	//returnIfErrC(false, !ele);
	return ele?ele->Open(para):false;
}
EXTERNC void smf_close(void* hd) {	//dbgTestPXL("%p", hd);
	auto ele = To<Service>(hd, IS_Service);
	if (ele){
		ele->Close();
	}
}
///
EXTERNC bool smf_process(void* hd, smf_frame_t* input, smf_frame_t* output) {
	auto ele = To<Element>(hd, IS_Element);
	//returnIfErrC(false, !ele);
	return ele?ele->Process((Frame*)input, (Frame*&)output):false;
}

EXTERNC bool smf_output(void* hd, smf_frame_t* output) {
	auto ele = To<Element>(hd, IS_Element);
	return ele?ele->Output((Frame*&)output):false;
}
EXTERNC bool smf_input(void* hd, smf_frame_t* input) {
	auto ele = To<Element>(hd, IS_Element);
	return ele?ele->Input((Frame*)input):false;
}
EXTERNC bool smf_pull(void* hd, smf_frame_t* output) {
	auto ele = To<Element>(hd, IS_Element);
	return ele ? ele->Pull((Frame*&)output) : false;
}
EXTERNC bool smf_push(void* hd, smf_frame_t* input) {
	auto ele = To<Element>(hd, IS_Element);
	return ele ? ele->Push((Frame*)input) : false;
}
//
EXTERNC int smf_get_input_minsize(void* hd) {
	auto port = iPort(hd);
	return port ? port->_min : 0;
}
EXTERNC int smf_get_input_maxsize(void* hd) {
	auto port = iPort(hd);
	return port ? port->_max : 0;
}
EXTERNC bool smf_register_input(void* hd, CbFrame cb, void* priv) {
	auto port = iPort(hd);
	return port ? port->Set(new HookCb(cb, priv),true) : 0;
}
EXTERNC int smf_get_output_minsize(void* hd) {
	auto port = oPort(hd);
	return port ? port->_min : 0;
}
EXTERNC int smf_get_output_maxsize(void* hd) {
	auto port = oPort(hd);
	return port ? port->_max : 0;
}
EXTERNC bool smf_register_output(void* hd, CbFrame cb, void* priv) {
	auto port = oPort(hd);
	return port ? port->Set(new HookCb(cb, priv),true) : 0;
}
