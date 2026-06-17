#include "Filter.h"
#include "SmfHash.h"
#include "smf_debug.h"
#include "SmfPorting.h"
using namespace smf;
//Filter::~Filter() {
//
//}
Filter::Filter(){
	_flags.Set(IS_Filter);
}

bool Filter::Schedule() {
	CleanError();
	Frame* ifrm = 0;
	if (!_iport->Pull(ifrm)) {
		if (!processError(ifrm,0)) {
			return false;
		}
	}
	Frame* ofrm = _oport->InitFrame();
	if (!ofrm && !processError(ifrm, ofrm)) {
		return false;
	}
	//
	if (ifrm && (ifrm->size || ifrm->flags)) {
		if (ofrm) {
			ofrm->index = ifrm->index;
			ofrm->flags = ifrm->flags;
			ofrm->ext = ifrm->ext;
			ofrm->timestamp = ifrm->timestamp;
			ofrm->dts = ifrm->dts;
			ofrm->gts = ifrm->gts;
		}
		auto rst = true;
		{
			MeasureCheck mc(this);
			rst = processFrame(ifrm, ofrm);
		}
		if (!rst && !processError(ifrm, ofrm)) {
			return false;
		}
	}
	if (!_oport->Push(ofrm)) {
		if (!processError(ifrm,ofrm)) {
			return false;
		}
	}
	if (ifrm) {
		_iport->PostFrame(ifrm, this);
	}
	return true;
}

bool Filter::processFrame(Frame* ifrm, Frame*& ofrm) {
	return false;
}

bool Filter::generateFrame(Frame*& ofrm) {
	Frame* ifrm = 0;
	if (!_iport->Pull(ifrm)) {
		if (!processError(ifrm,ofrm)) {
			return false;
		}
	}
	ofrm = _oport->InitFrame(ofrm);
	if (!processError(ifrm, ofrm)) {
		return false;
	}
	if (ifrm && (ifrm->size || ifrm->flags)) {
		if (ofrm) {
			ofrm->index = ifrm->index;
			ofrm->flags = ifrm->flags;
			ofrm->ext = ifrm->ext;
			ofrm->timestamp = ifrm->timestamp;
			ofrm->dts = ifrm->dts;
			ofrm->gts = ifrm->gts;
		}
		auto rst = true;
		{
			MeasureCheck mc(this);
			rst = processFrame(ifrm, ofrm);
		}
		if (!rst && !processError(ifrm, ofrm)) {
			ifrm->size = 0;
			ifrm->offset = 0;
			_iport->PostFrame(ifrm, this);
			return false;
		}
		_iport->PostFrame(ifrm, this);
	}
	return true;
}
bool Filter::receiveFrame(Frame* ifrm) {
	Frame* ofrm = _oport->InitFrame();
	if (!processError(ifrm, ofrm)) {
		return false;
	}
	if (ofrm) {
		ofrm->index = ifrm->index;
		ofrm->flags = ifrm->flags;
		ofrm->ext = ifrm->ext;
		ofrm->timestamp = ifrm->timestamp;
		ofrm->dts = ifrm->dts;
		ofrm->gts = ifrm->gts;
	}
	{
		auto rst = true;
		{
			MeasureCheck mc(this);
			rst = processFrame(ifrm, ofrm);
		}
		if (!rst && !processError(ifrm, ofrm)) {
			return false;
		}
	}
	if (!_oport->Push(ofrm)) {
		if (!processError(ifrm,ofrm)) {
			return false;
		}
	}
	return true;
}
