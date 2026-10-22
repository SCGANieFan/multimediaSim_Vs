#include "VideoFormatList.h"
#include "VideoFormat.h"
#include "MediaVideo.h"
#include "SmfHash.h"

using namespace smf;
using namespace smf::video;

VideoFormatList::VideoFormatList() {
	memset(_items, 0, sizeof(_items));
}

void VideoFormatList::Add(uint32_t id, Func func) {
	//dbgTestPXL("%p,%08x", item_, item_ ? item_->_id : 0);
	returnIfErrCS0(_cnt>=MAX_ITEMS,"%08x,%p",id,func);
	auto& item = _items[_cnt++];
	item._id = id;
	item._func = func;
}

VideoFormatList::Func VideoFormatList::Find(uint32_t id) {
	//dbgTestPXL("%08x,%d", id, _cnt);
	if (id == __copy)return &Copy;
	for (int i = 0; i < _cnt; i++) {
		auto& item = _items[i];
		if (item._id == id) {
			return item._func;
		}
	}
	dbgWarnPXL("%08x,%d", id, _cnt);
	return 0;
}
VideoFormatList::Func VideoFormatList::Find(VideoFormat*vf) {
	return Find(vfid32((const char*)&vf->_icodec, vf->_ifmt, (const char*)&vf->_ocodec, vf->_ofmt, vf->_scale, vf->_crop));
}

bool VideoFormatList::Copy(VideoFormat* vf, void* idata, void*& odata) {
	returnIfErrC(false, !vf->_isize);
	returnIfErrC(false, !vf->_osize);
	returnIfErrC(false, vf->_isize != vf->_osize);
	memcpy(odata, idata, vf->_isize);
	return true;
}
