#pragma once
#include <stdint.h>
#include <atomic>

namespace smf {
	class Locker {
	public:
		Locker(volatile uint8_t& lock, uint32_t timeout = 0);
		Locker(volatile uint8_t& lock, void*& tid, uint32_t timeout = 0);
		~Locker();
	protected:
		volatile uint8_t* _lock = 0;
		void** _ptid = 0;
	public:
		operator bool()const { return _lock; }
		bool Get()const;
		bool Lock();
		bool Lock(uint32_t timeout);
		void Unlock();
	};

	class AtomicLocker {
	protected:
		std::atomic<bool>* _lock;
	public:
		AtomicLocker(std::atomic<bool>* lock) : _lock(lock) { Acquire(); }
		AtomicLocker(std::atomic<bool>& lock) : _lock(&lock) { Acquire(); }
		~AtomicLocker() { Release(); }
	protected:
		void Acquire();
		void Release();
	};
}
