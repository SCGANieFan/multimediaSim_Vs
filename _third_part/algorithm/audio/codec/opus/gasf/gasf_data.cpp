#include "gasf_data.h"
using namespace GASF_NAME_SPACE;

void GasfData_c::Init(void* buf, uint32_t max, uint32_t size) {
    _buf = buf;
    _max = max;
    _size = size;
    _offset = 0;
    _flag = 0;
}

void GasfData_c::Deinit() {
    _buf = 0;
    _size = 0;
    _max = 0;
    _offset = 0;
    _flag = 0;
}
uint32_t GasfData_c::Append(void* buf, uint32_t size) {
    if (!buf || !size) return 0;
    uint32_t appendSize = LeftSize();
    appendSize = appendSize < size ? appendSize : size;
    gasf_memcpy(LeftData(), buf, appendSize);
    _size += appendSize;
    return appendSize;
}
uint32_t GasfData_c::Append(uint32_t size) {
    if (!size) return 0;
    uint32_t appendSize = LeftSize();
    appendSize = appendSize < size ? appendSize : size;
    _size += appendSize;
    return appendSize;
}
uint32_t GasfData_c::Used(uint32_t size) {
    if (!size) return 0;
    uint32_t usedSize = size;
    usedSize = _size > usedSize ? usedSize : _size;
    _size -= usedSize;
    _offset += usedSize;
    return usedSize;
}
uint32_t GasfData_c::ClearUsed() {
    if (!_offset) return 0;
    uint32_t clearByte = _offset;
    gasf_memmove(Buf(), Data(), Size());
    _offset = 0;
    return clearByte;
}

