#pragma once
#include <stdint.h>

namespace smf {
	class Flags32	{
	protected:
		uint32_t _flags=0;
	public:
		uint32_t Get(uint32_t mask = 0xffffffff)const;
	public:
		void Set(uint32_t val);
		void Set(uint32_t val, uint32_t mask);
		void Set(uint32_t mask, bool val);
		void Clear(uint32_t mask);
		operator uint32_t() const { return _flags; }
	public:
		bool GetBit(int bit)const;
		void SetBit(int bit);
		void ClearBit(int bit);
	public:
		uint32_t Get(uint8_t offset, uint8_t bits)const;
		void Set(uint8_t offset, uint8_t bits, uint32_t val);
	public:
		bool Check(uint32_t mask) const;
		bool CheckAny(uint32_t mask) const;
		bool CheckAll(uint32_t mask) const;
		bool CheckAndSet(uint32_t mask);
		bool CheckAndClear(uint32_t mask);
	public:
		template<class T>
		void Set(T mask, bool val) { Set((uint32_t)mask, val); }
		template<class T>
		bool Check(T mask) const { return Check((uint32_t)mask); }
		template<class T>
		bool CheckAny(T mask) const { return Check((uint32_t)mask); }
		template<class T>
		bool CheckAll(T mask) const { return CheckAll((uint32_t)mask); }
		template<class T>
		bool CheckAndSet(T mask) { return CheckAndSet((uint32_t)mask); }
		template<class T>
		bool CheckAndClear(T mask) { return CheckAndClear((uint32_t)mask); }
	};
}

