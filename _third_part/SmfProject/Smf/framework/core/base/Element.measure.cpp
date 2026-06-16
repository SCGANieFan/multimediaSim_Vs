#include "Element.h"
#include "SmfPorting.h"
#include "smf_debug.h"
using namespace smf;

MeasureCheck::MeasureCheck(Element* ele) {
	if (ele->_processTimeout) {
		_ele = ele;
		_ms = get_ms();
	}
}
MeasureCheck::~MeasureCheck() {
	if (_ele && _ele->_processTimeout) {
		auto ms = get_ms();
		if (ms > _ms + _ele->_processTimeout) {
			dbgWarnXL("timeout:%s(%u)%dms=%u-%u", _ele->Name(), _ele->_processTimeout, (int)(ms - _ms), ms, _ms);
		}
	}
}
