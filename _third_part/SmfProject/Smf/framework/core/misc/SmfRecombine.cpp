#include "SmfRecombine.h"
#include "string.h"
#include "smf_debug.h"

using namespace smf;
SmfRecombine::SmfRecombine(Object*parent, int max) {
	returnIfErrC0(!parent);
	returnIfErrC0(!max);
	Parent(parent);
	_max = max;
}
SmfRecombine::~SmfRecombine() {
	Free(_buff);
	Free(_fbuff);
}

bool SmfRecombine::Recombine(Frame* frm) {
	returnIfErrC(false, !_max);
	if (frm->flags & SMF_FRAME_INCOMPLETE) {
		//dbgInfoDump(frm->buff, 32);
		if (!_buff) {
			_buff = (char*)Alloc(_max);
			returnIfErrC(false, !_buff);
		}
		//copy incompleted frame.
		returnIfErrC(false, _size + frm->size > _max);
		memcpy(_buff + _size, (char*)frm->buff + frm->offset, frm->size);
		_size += frm->size;
		//
		frm->size = 0;
		return true;
	}
	else if (_size) {
		//copy last incompleted frame
		returnIfErrC(false, _size + frm->size > _max);
		memcpy(_buff + _size, (char*)frm->buff + frm->offset, frm->size);
		_size += frm->size;
		//dbgTestPDL(_size);
		//return recombined frame
		//frm->buff = _buff;
		if (_size > frm->max) {
			if (!_fbuff) {
				_fbuff = (char*)Alloc(_max);
				returnIfErrC(false, !_fbuff);
			}
			memcpy(_fbuff, _buff, _size);
			frm->buff = _fbuff;
			frm->max = _max;
		}
		else {
			memcpy(frm->buff, _buff, _size);
			frm->max = _max;
		}
		//dbgInfoDump(frm->buff, 32);
		frm->size = _size;
		frm->offset = 0;
		//reset buff size
		_size = 0;
		//dbgInfoPDL(frm->size);
		frm->flags &=~SMF_FRAME_INCOMPLETE;
		return true;
	}
	else {//completed frame, not to recombine frame;
		return true;
	}
}
