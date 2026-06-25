#include <memory>
#include <cstring>
#include "BitStream.h"

#define SWAP32(v) (((uint32_t)(v)<<24)|((uint32_t)(v)>>24)|(((uint32_t)(v)<<8)&0x00ff0000)|(((uint32_t)(v)>>8)&0x0000ff00))
using namespace smf;

void BsReader::Init(void* buff0, int size) {
	uint8_t* buff = (uint8_t*)buff0;
	_begin = buff;
	_end = buff + size;
	int align = (uint32_t)buff & 3;	
	_ptr = (uint32_t*)(buff - align);
	_bits = (4 - align) << 3;
	uint32_t v = *_ptr++;
	_cache = SWAP32(v) << (32 - _bits);
}
unsigned BsReader::GetLeftBits() const {
	return (_end - (uint8_t*)_ptr) + _bits;
}
unsigned BsReader::GetUsedBits() const {
	return ((uint8_t*)_ptr - _begin) + (32 - _bits);
}
unsigned BsReader::Read(int bits) {
	if (!bits || bits > 32) {
		return 0;
	}
	if ((uint8_t*)_ptr >= _end && _bits < bits) {
		return 0;
	}
	if (bits <= _bits) {
		uint32_t val = _cache >> (32 - bits);
		_cache <<= bits;
		_bits -= bits;
		return val;
	}
	else {
		uint32_t v0 = _cache >> (32 - bits);
		uint32_t v1 = *_ptr++;
		v1 = SWAP32(v1);
		_cache = v1 << (bits - _bits);
		_bits = 32 + _bits - bits;
		return v0 | (v1 >> _bits);
	}
}
unsigned BsReader::Peak(int bits) const {
	if ((uint8_t*)_ptr >= _end) {
		return 0;
	}
	if (!bits || bits > 32) {
		return 0;
	}
	if (bits <= _bits) {
		return _cache >> (32 - bits);
	}
	else {
		uint32_t v0 = _cache >> (32 - bits);
		uint32_t v1 = *_ptr;
		v1 = SWAP32(v1);
		return v0 | (v1 >> (32 + _bits - bits));
	}
}

void BsWriter::Init(void* buff0, int size) {
	uint8_t* buff = (uint8_t*)buff0;
	_begin = buff;
	_end = buff + size;
	_ptr = (uint32_t*)buff;
	_bits = 0;
	_cache = 0;
}
unsigned BsWriter::GetLeftBits() const {
	return (_end - (uint8_t*)_ptr) + (32 - _bits);
}
unsigned BsWriter::GetUsedBits() const {
	return ((uint8_t*)_ptr - _begin) + _bits;
}

bool BsWriter::Write(uint32_t val, int bits) {
	if (!bits)return true;
	if ((uint8_t*)_ptr + ((_bits + bits + 7) >> 3) >= _end)return false;
	auto left = 32 - _bits;
	if (bits < left) {
		_cache = (_cache >> bits) | (val << (32 - bits));
		_bits += bits;
	}
	else{
		_cache = (_cache >> left) | (val << (32 - left));
		auto v = SWAP32(_cache);
		if (!((uint32_t)_ptr & 3u)) {
			*_ptr++ = v;
			_bits = bits - left;
			_cache = val << (32 - _bits);
		}else{
			auto x = 4 - ((uint32_t)_ptr & 3);
			memcpy(_ptr, &v, x);
			_bits = (4 - x) << 3;
			_cache <<= (x << 3);
			_ptr = (uint32_t*)((uint32_t)_ptr & ~3u);
		}
	}
	return true;
}

void BsWriter::Flush() {
	auto v = SWAP32(_cache);
	memcpy(_ptr, &v, ((_bits + 7) >> 3));
}


bool ExponentialColumbianEntropy::Encode(BsWriter& bs, uint32_t v) {
	auto vv = v + 1;
	auto m = 0;
	v = vv;
	while (vv) {
		m++; vv >>= 1;
	}
	return bs.Write(v, m * 2 - 1);
}
uint32_t ExponentialColumbianEntropy::Decode(BsReader& bs) {
	int c = 0;
	while (!bs.Read(1)) {
		c++;
	}
	uint32_t v = bs.Read(c);
	return (v | (1 << c)) - 1;
}

bool ExponentialColumbianEntropy::Encode(BsWriter& bs, int32_t v) {
	uint32_t vv = v >= 0 ? v : -v;
	auto m = 0;
	auto x = vv;
	while (vv) {
		m++; vv >>= 1;
	}
	x <<= 1;
	x |= v >= 0 ? 0 : 1;
	return bs.Write(x, m * 2);
}
int32_t ExponentialColumbianEntropy::Decodes(BsReader& bs) {
	int c = 0;
	while (!bs.Read(1)) {
		c++;
	}
	int v = bs.Read(c - 1);
	v |= (1 << c);
	return bs.Read(1) ? -v : v;
}
