#include "ShmFifoElement.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "SmfLayout.h"
#include <stdio.h>
#include <stdlib.h>
using namespace smf;

ShmFifoElement::ShmFifoElement(Element* this_, bool source)
	:_ele(this_)
	, _isSource(source)
{
	_isCacheable = true;
}

bool ShmFifoElement::open() {
	auto cacheable = _isCacheable;
	_shm = (ShmFifo*)_shm;
	//dbgTestPXL("%p,%d,%d,%d", _shm, _fifo_max, _buff_max, _shmSize);
	if (!_shm) {
		if (!_keys && _ele)_keys = _ele->ID();
		returnIfErrC(false, !_keys);
		if (_buff_max && _fifo_max) {
			_shm = ShmFifo::Open((const char*)&_keys, _buff_max, _fifo_max, _sharedIndex, false, _align);
		}
		else {
			_shm = ShmFifo::Open((const char*)&_keys, _timeout, _sharedIndex);
		}
		dbgInfoPXL("%s,%p,%d,%d,%u", &_keys, _shm, _fifo_max, _buff_max, _sharedIndex);
		returnIfErrC(false, !_shm);
	}
	auto ele_name = _ele?_ele->Name():"null";
	if (_isSource) {
		returnIfErrC(false, !_shm->InitRead(cacheable, _noCopy, _partread));
		if (_noForward) {
			_shm->SetRead(ShmFifo::set_e::noForward, (void*)true);
		}
		if (_noBackward) {
			_shm->SetRead(ShmFifo::set_e::noBackward, (void*)true);
		}
		if (_noCopy && _ele) {
			_ele->oPort()->_noAllocFrameBuff = true;
		}
		if (!_ele->oPort()->_media) {
			IMedia* media = _shm->Read(_timeout);
			if (media && _ele) {
				_ele->oPort()->Set(media);
				//dbgTestPXL("[%s]wait media success", &_keys);
			}
			else {
				dbgWarnPXL("[%s]wait media timeout(%d ms)", &_keys, _timeout);
			}
		}
	}
	else {
		returnIfErrC(false, !_shm->InitWrite(cacheable, _noCopy, _isRing));
		if (_noForward) {
			_shm->SetWrite(ShmFifo::set_e::noForward, (void*)true);
		}
		if (_noBackward) {
			_shm->SetWrite(ShmFifo::set_e::noBackward, (void*)true);
		}
		if (_ele) {
			auto media = _ele->iMedia();
			if (media) {
				if (_shm->Write(media)) {
					//dbgTestPXL("[%s]write media success", &_keys);
				}
				else {
					dbgWarnPXL("[%s]write media failed!", &_keys);
				}
			}
			else {
				dbgErrPXL("[%s]no media!", &_keys);
			}
		}
		else {
			dbgErrPXL("no ele");
		}
	}
	return true;
}
bool ShmFifoElement::close() {
	//dbgTestPL();
	//dbgInfoPXL("%s,%p", &_keys, _shm);
	if (_shm && !_noAlloc) {
		ShmFifo::Close(_shm, _sharedIndex);
	}
	_shm = 0;
	return true;
}

bool ShmFifoElement::generateFrame(Frame* frm) {
	returnIfErrC(false, !_shm);
	bool rst = _shm->Read(*frm);
	if (rst && _ele && !_ele->oPort()->Media() && frm->media) {
		_ele->oPort()->Set(frm->Media());
	}
	if (rst && frm) {
		frm->media = (smf_media_def_t*)_ele->oPort()->_media;
	}
	if (!rst && _sleep_ms) {
		sleep_for(_sleep_ms);
	}
	//dbgTestPXL("[%u]%u,%08x", frm->index, frm->size, frm->flags);
	return rst;
}

bool ShmFifoElement::receiveFrame(Frame* frm) {
	returnIfErrC(false, !_shm);
	frm->flags &= ~SMF_FRAME_IS_RESERVE;
	auto rst = _shm->Write(*frm);
	if (!rst ) {
		if(!frm->size && frm->flags) frm->flags |= SMF_FRAME_IS_RESERVE;
		if(_sleep_ms)sleep_for(_sleep_ms);
	}
	//dbgTestPXL("[%u]%u,%08x", frm->index, frm->size, frm->flags);
	return rst;
}

bool ShmFifoElement::set(const char* script) {
	return Layout(script).Parse([](void* priv, const char* keys, void* vals_, char rst) {
		((ShmFifoElement*)priv)->set(Hash(keys), (void*)vals_);
		}, this);
}
bool ShmFifoElement::set(uint32_t keys, void* vals) {
	switch(keys){
	case Hash("timeout"): _timeout = (int)vals; dbgTestPL();return true;
	case Hash("shm"): {
		_shm = (ShmFifo*)vals;
		if (_shm) _noAlloc = true;
		return true;
	}
	//case Hash("shmSize"): _shmSize = (uint32_t)vals; return true;
	case Hash("keys"): _keys = fcc64((char*)vals); return true;
	case Hash("sleep"): _sleep_ms = (int)vals; return true;
	case Hash("fmax"):
	case Hash("fifomax"): _fifo_max = (int)vals; return true;
	case Hash("bmax"):
	case Hash("buffmax"): _buff_max = (int)vals; return true;
	case Hash("cacheable"): _isCacheable = (bool)(int)vals; return true;
	case Hash("isRing"): _isRing = (bool)(int)vals; return true;
	case Hash("isReader"): _isReader = (bool)(int)vals; return true;
	case Hash("noCopy"): _noCopy = (bool)(int)vals; return true;
	case Hash("noForward"): _noForward = (bool)(int)vals; return true;
	case Hash("noBackward"): _noBackward = (bool)(int)vals; return true;
	case Hash("part_r"): _partread = (bool)(int)vals; return true;
	case Hash("sidx"): _sharedIndex = (uint8_t)(int)vals; return true;
	case Hash("ignMedia"): _ignoreMedia  = (bool)(int)vals; return true;
	case Hash("align"): _align  = (uint8_t)(uint32_t)vals; return true;
	default:break;
	}
	return false;
}
bool ShmFifoElement::get(uint32_t keys, void* vals) const {
	switch (keys) {
	case Hash("fmax"):*(int*)vals = _fifo_max; return true;
	case Hash("bmax"):*(int*)vals = _buff_max; return true;
	case Hash("shm"):*(ShmFifo**)vals = _shm; return true;
	default:return false;
	}
}

//
bool ShmFifoElement::WaitMedia(uint32_t timeout) {
	returnIfErrC(false, !_shm);
	returnIfErrC(false, !_isSource);
	if (!timeout)timeout = _timeout;
	IMedia* media = _shm->Read(timeout);
	if(media && _ele)_ele->oPort()->Set(media);
	returnIf(true, _ignoreMedia);
	return media;
}

char* ShmFifoElement::print(char*ptr, char*end)const {
	if (_shm) {
		*ptr++ = ',';
		*ptr++ = _isSource ? 'i' : 'o';
		*ptr++ = '{';
		ptr = _shm->Print(ptr, end);
		*ptr++ = '}';
		//ptr = snprintf(ptr, end, ",%c{%08x(%u,%u)%u(%u,%u),%u/%u}"
		//	, _isSource?'i':'o'
		//	, _shm, _shm->_fifo_max, _shm->_data_size
		//	,_shm->_w.fidx - _shm->_r.fidx, _shm->_r.fidx, _shm->_w.fidx
		//	, (uint32_t)_shm->_r.bidx, (uint32_t)_shm->_w.bidx
		//);
	}
	return ptr;
}

bool ShmFifoElement::From(shmcfg_t& cfg) {
	_timeskip = cfg._timeskip;
	_timeout = cfg._timeout;
	_keys = cfg._keys;
	_buff_max = cfg._buff_max;
	_fifo_max = cfg._fifo_max;
	_sleep_ms = cfg._sleep_ms;
	_sharedIndex = cfg._sharedIndex;
	_isCacheable = cfg._isCacheable;
	_noCopy = cfg._noCopy;
	_isReader = cfg._isReader;
	_isRing = cfg._isRing;
	_noAlloc = cfg._noAlloc;
	_partread = cfg._partread;
	return true;
}
bool ShmFifoElement::To(shmcfg_t& cfg) const {
	cfg._timeskip = _timeskip;
	cfg._timeout = _timeout;
	cfg._keys = _keys;
	cfg._buff_max = _buff_max;
	cfg._fifo_max = _fifo_max;
	cfg._sleep_ms = _sleep_ms;
	cfg._sharedIndex = _sharedIndex;
	cfg._isCacheable = _isCacheable;
	cfg._noCopy = _noCopy;
	cfg._isReader = _isReader;
	cfg._isRing = _isRing;
	cfg._noAlloc = _noAlloc;
	cfg._partread = _partread;
	return true;
}
