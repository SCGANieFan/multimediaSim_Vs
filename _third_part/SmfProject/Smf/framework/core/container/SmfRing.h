#pragma once
#include "SmfFifo.h"
//#include "SmfFifoBuffer.h"
#include "smf_debug.h"
namespace smf {
	template<class Base>
	class TRing : protected Base {
	public:
		using ring_t = smf_ring_t;
	public:
		using Base::Base;
		using Base::Initialize;
		using Base::Uninit;
		using Base::IsValid;
		using Base::Max;
		using Base::Data;
		using Base::CreateReader;
		using typename Base::Reader;
	public:
		ring_t* Ring()const { return (ring_t*)Base::_fifo; }
		uint32_t Size()const { return (Base::_fifo->wi > Base::_fifo->max) ? Base::_fifo->max : (uint32_t)Base::_fifo->wi; }
		uint64_t Index()const { return Base::_fifo->wi; }
		int Seek(int offset) { return Base::WriteSeek(offset, true); }
	public:
		uint32_t Write(const void* data, uint32_t size, bool all = false, bool cache = false) { return Base::Write(data, size, all, cache, true); }
		uint32_t WritePointer(void*& data)const { return Base::WritePointer(data, true); }
		int WriteSeek(int offset) { return Base::WriteSeek(offset, true); }

	public:
		int Size(uint64_t ri)const { return Base::Size(ri); }
		int Left(uint64_t ri)const { return Base::Left(ri); }
		bool Full(uint64_t ri) const { return Base::Full(ri); }
		bool Empty(uint64_t ri) const { return Base::Empty(ri); }
	public:
		int ReadSeek(uint64_t& ri, int offset, bool check = true) const { return Base::ReadSeek(ri,offset,check); }
		int ReadSync(uint64_t& ri, int offset = 0, bool check = true) const { return Base::ReadSync(ri,offset,check); }
		int ReadSyncLast(uint64_t& ri) const { return Base::ReadSyncLast(ri); }
		int ReadCheck(uint64_t& ri, bool sync = false) const { return Base::ReadCheck(ri,sync); }
		uint32_t Read(uint64_t& ri, void* data, uint32_t size, bool all, bool cache) { return Base::Read(ri,data,size,all,cache); }
		uint32_t ReadWithCache(uint64_t& ri, void* data, uint32_t size, bool all) { return Base::ReadWithCache(ri,data,size,all); }
		uint32_t Peak(uint64_t& ri, void* data, uint32_t size, bool all, bool cache)const { return Base::Peak(ri,data,size,all,cache); }
		uint32_t PeakWithCache(uint64_t& ri, void* data, uint32_t size, bool all) const { return Base::PeakWithCache(ri,data,size,all); }
		uint32_t ReadPointer(uint64_t& ri, void*& data, bool cache = false)const { return Base::ReadPointer(ri,data,cache); }
	};

	template<class T>
	using SmfRing = TRing<SmfFifo<T>>;
	template<class T, int _max>
	using SmfRingS = TRing<SmfFifoS<T, _max>>;
	template<class T>
	using SmfRingD = TRing<SmfFifoD<T>>;
}
