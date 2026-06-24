#include "SmfBuffer.h"
#include "SmfDef.h"

using namespace smf;

void SmfBuffer::reset(int size, int align) {
	_offset = 0;
	VoidBuffer::reset(size, align);
}

bool SmfBuffer::append(const void* data, int size) {
	//returnIfErrC(false, _offset + size > _size);
	if (_offset + size > _size) {
		auto len = _size + _size;
		if (len < _offset + size)len = _offset + size;
		auto ptr = Realloc(_data, len);
		returnIfErrC(false, !ptr);
		_data = ptr;
		_size = len;
	}
	memcpy((char*)_data + _offset, data, size);
	_offset += size;
	return true;
}
bool SmfBuffer::append(const char* str) {
	return append(str, strlen(str));
}
bool SmfBuffer::append(char str) {
	return append(&str, 1);
}
