#include "gaapi_data.h"
using namespace plc_gaapi_ns;

void GaapiData_c::Init(void* buf, uint32_t max) {
    _buf = buf;
    _size = 0;
    _max = max;
    _offset = 0;
    _flag = 0;
}
void GaapiData_c::Init(void* buf, uint32_t size, uint32_t max) {
    size = size > max ? max : size;
    _buf = buf;
    _size = size;
    _max = max;
    _offset = 0;
    _flag = 0;
}
void GaapiData_c::Deinit() {
    _buf = 0;
    _size = 0;
    _max = 0;
    _offset = 0;
    _flag = 0;
}
uint32_t GaapiData_c::Append(void* buf, uint32_t size) {
    if (!buf || !size) return 0;
    uint32_t appendSize = LeftSize();
    appendSize = appendSize < size ? appendSize : size;
    gaapi_memcpy(LeftData(), buf, appendSize);
    _size += appendSize;
    return appendSize;
}
uint32_t GaapiData_c::Append(uint32_t size) {
    if (!size) return 0;
    uint32_t appendSize = LeftSize();
    appendSize = appendSize < size ? appendSize : size;
    _size += appendSize;
    return appendSize;
}
uint32_t GaapiData_c::Used(uint32_t size) {
    if (!size) return 0;
    uint32_t usedSize = size;
    usedSize = _size > usedSize ? usedSize : _size;
    _size -= usedSize;
    _offset += usedSize;
    return usedSize;
}
uint32_t GaapiData_c::ClearUsed() {
    if (!_offset) return 0;
    uint32_t clearByte = _offset;
    gaapi_memmove(Buf(), Data(), Size());
    _offset = 0;
    return clearByte;
}

