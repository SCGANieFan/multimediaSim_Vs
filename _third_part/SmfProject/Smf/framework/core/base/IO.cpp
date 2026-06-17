#include "IO.h"
#include "smf_debug.h"

using namespace smf;

IO::IO() {
	_flags.Set(IS_IO);
	Parent(0);
}
bool IO::Open(void* para) {
	returnIfTestC(true, IsOpen());
	_flagsExt.Set(ExFlags::IS_Open);
	return true;
}
bool IO::Close() {
	returnIf(true, !IsOpen());
	_flagsExt.Clear(ExFlags::IS_Open);
	return true;
}
bool IO::IsOpen() const {
	return _flagsExt.CheckAll(ExFlags::IS_Open);
}
bool IO::Seek(int offset, Position pos) { return false; }

unsigned IO::GetSize() const {
	auto pos = GetOffset();
	returnIfErrC(false, !((IO*)this)->Seek(0, Position::end));
	auto size = GetOffset();
	returnIfErrC(false, !((IO*)this)->Seek(pos, Position::front));
	return size;
}
unsigned IO::GetOffset() const { return 0; }
unsigned IO::Read(void* buff, unsigned size) { return 0; }
unsigned IO::Write(void* buff, unsigned size) { return 0; }

bool IO::Reads(IOReadParam_t* paras, int count) { 
	for (int i = 0; i < count; ++i) {
		auto& para = paras[i];
		// dbgInfoPXL("[%d]%p,%d,%d,%d", i, para.ptr, para.size, para.oft, para.where);
		returnIfErrC(false, !para.ptr || !para.size);
		returnIfErrC(false, !Seek(para.oft, (Position)para.where));
		void* data = (void*)para.ptr;
		auto rsize = Read(data, para.size);
		if (rsize != para.size) {
			dbgErrPXL("[%u]%p,%u/%u",i, data, rsize, para.size);
		}
	}
	return true;
}