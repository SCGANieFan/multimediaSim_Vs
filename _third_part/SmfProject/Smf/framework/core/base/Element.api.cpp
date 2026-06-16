#include "Element.h"
#include "Hook.h"
#include "smf_debug.h"
using namespace smf;
//Filter
bool Element::Process(Frame* ifrm,Frame*& ofrm) {
	returnIfErrC(false, !_iport);
	returnIfErrC(false, !_oport);
	ifrm = _iport->InitFrame(ifrm);
	if (_iport->_hook)_iport->_hook->Hook(ifrm);
	ofrm = _oport->InitFrame(ofrm);
	returnIfErrC(false, !ifrm || !ofrm);
	if (Root().Error().err & SMF_ERROR_MASK_CHIP) {//[chip fail]skip processFrame
		if (ifrm) ifrm->size = 0;
		if (ofrm) ofrm->size = ofrm->max;
	}
	else if (!processFrame(ifrm, ofrm)) {
		if (!processError(ifrm, ofrm))
			return false;
	}
	_iport->PostFrame(ifrm, this);
	if (_oport->_hook)_oport->_hook->Hook(ofrm);
	return true;
}
//source
bool Element::Output(Frame*& frm) {
	returnIfErrC(false, !_oport);
	frm = _oport->InitFrame(frm);
	if (Root().Error().err & SMF_ERROR_MASK_CHIP_CPU) {//[chip fail]skip generateFrame
		if (frm) frm->size = frm->max;
	}
	else if (!generateFrame(frm)) {
		if (!processError(0, frm))
			return false;
	}
	if (_oport->_hook)_oport->_hook->Hook(frm);
	return true;
}
bool Element::Push(Frame* frm) {
	returnIfErrC(false, !frm);
	returnIfErrC(false, !_oport);
	frm = _oport->InitFrame(frm);
	return _oport->Push(frm);
}
//sink
bool Element::Input(Frame* frm) {
	returnIfErrC(false, !_iport);
	frm = _iport->InitFrame(frm);
	if (_iport->_hook)_iport->_hook->Hook(frm);
	if (Root().Error().err & SMF_ERROR_MASK_CHIP_CPU) {//[chip fail]skip receiveFrame
		if (frm) frm->size = 0;
	}
	else if (!receiveFrame(frm)) {
		if (!processError(frm, 0))
			return false;
	}
	return _iport->PostFrame(frm,this);
}
bool Element::Pull(Frame*& frm) {
	returnIfErrC(false, !_iport);
	return _iport->Pull(frm) && _iport->PostFrame(frm, this);
}
