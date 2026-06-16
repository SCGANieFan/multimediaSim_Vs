#include "Element.h"
#include "smf_debug.h"
using namespace smf;
//source
bool Element::DislinkOutput() {
	returnIfTestC(true, !_oport);
	_oport->DisLink();
	return true;
}
bool Element::LinkTo(const char* name) {
	return Parent() ? LinkTo((Element*)Parent()->Child(name)) : false;
}
bool Element::LinkTo(Element* ele) {
	returnIfErrC(false,!ele);
	dbgTestPXL("%s->%s", Name(), ele->Name());
	beforeLink(ele->_iport, true);
	returnIfErrCS(false, !_oport, "%s", Name());
	ele->beforeLink(_oport, true);
	returnIfErrC(false, !ele->_iport);
	return ele->_iport->Link(_oport);
}
