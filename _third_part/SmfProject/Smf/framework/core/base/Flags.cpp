#include "Flags.h"
using namespace smf;

uint32_t Flags32::Get(uint32_t mask) const {
	return _flags & mask;
}
void Flags32::Set(uint32_t val) {
	_flags |= val;
}
void Flags32::Set(uint32_t val, uint32_t mask) {
	_flags = (_flags & ~mask) | (val & mask);
}
void Flags32::Set(uint32_t mask, bool val) {
	_flags = (_flags & ~mask) | (val ? mask : 0);
}
void Flags32::Clear(uint32_t mask) {
	_flags &= ~mask;
}

bool Flags32::GetBit(int bit)const {
	return _flags & (1u << bit);
}
void Flags32::SetBit(int bit) {
	_flags |= 1u << bit;
}
void Flags32::ClearBit(int bit) {
	_flags &= (1u << bit) - 1; 
}

uint32_t Flags32::Get(uint8_t offset, uint8_t bits)const {
	return (_flags >> offset) & ((1u << bits) - 1);
}
void Flags32::Set(uint8_t offset, uint8_t bits, uint32_t val) {
	auto mask = ((1u << bits) - 1) << offset;
	_flags = (_flags & ~mask) | ((val << offset) & mask);
}

bool Flags32::Check(uint32_t mask) const {
	return _flags & mask;
}
bool Flags32::CheckAny(uint32_t mask) const {
	return _flags & mask;
}
bool Flags32::CheckAll(uint32_t mask) const {
	return (_flags & mask) == mask;
}
bool Flags32::CheckAndSet(uint32_t mask) {
	if ((_flags & mask) != mask) {
		Set(mask);
		return false;
	}
	return true;
}
bool Flags32::CheckAndClear(uint32_t mask) {
	if ((_flags & mask) == mask) {
		Clear(mask);
		return true;
	}
	return false;
}