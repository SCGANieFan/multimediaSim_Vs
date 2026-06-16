#include "Element.h"
#include "smf_debug.h"
using namespace smf;
///sink
bool Element::DislinkInput() {
	returnIfTestC(true, !_iport);
	_iport->DisLink();
	return true;
}
bool Element::LinkFrom(const char* name) {
	return Parent() ? LinkFrom((Element*)Parent()->Child(name)) : false;
}
bool Element::LinkFrom(Element* ele) {
	returnIfErrC(false, !ele);
	dbgTestPXL("%s<-%s", Name(), ele->Name());
	beforeLink(ele->_oport,true);
	returnIfErrCS(false, !_iport,"%s", Name());
	ele->beforeLink(_iport,true);
	returnIfErrCS(false, !ele->_oport,"%s",ele->Name());
	return ele->_oport->Link(_iport);
}

