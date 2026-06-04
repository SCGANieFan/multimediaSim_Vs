#pragma once
#include <stdint.h>
#include <SmfHash.h>
#include <ILoader.h>
#include <SimpleTable.h>
namespace smf {///Reflection
	class Object;
	class Reflection {	
	public:
		typedef Object* (*FuncCreate)();
		class item_t {
		public:
			uint32_t id;
			const char* name;
			FuncCreate creater;
			void* param;
		public:
			const char* Name()const { return name; }
			uint32_t ID() const { return id; }
			uint64_t IDs(int idx) const;
			uint64_t Type() const { return IDs(0); }
			uint64_t Keys() const { return IDs(1); }
		public:
			bool Load() const;
			bool Unload() const;
		};
	public:
		Reflection(item_t* items, int max);
	protected:
		item_t* _items = 0;
		int _count = 0;
	public:
#if 0
		static constexpr inline uint32_t Convert(unsigned v, char s) { return s == '-' ? v : ((v ^ (unsigned)Lower(s)) * _FNV_prime); }
		static constexpr inline uint32_t Convert(unsigned v, const char* s) {return *s ? Convert(Convert(v, *s), s + 1) : v; }
		static constexpr inline uint32_t Convert(const char* s0) { return Convert(_FNV_offset_basis, s0); }
		static constexpr inline uint32_t Convert(const char* s0, const char* s1) { return Convert(Convert(s0), s1); }
		static constexpr inline uint32_t Convert(const char* s0, const char* s1, const char* s2) { return Convert(Convert(s0, s1), s2); }
		static constexpr inline uint32_t Convert(const char* s0, const char* s1, const char* s2, const char* s3) { return Convert(Convert(s0, s1, s2), s3); }
		static constexpr inline uint32_t Convert(unsigned v, uint64_t s) { return (s & 0xff) ? Convert(Convert(v, (char)(s & 0xff)), static_cast<uint64_t>(s >> 8)) : v; }
		static constexpr inline uint32_t Convert(uint64_t s0) { return Convert(_FNV_offset_basis, s0); }
		static constexpr inline uint32_t Convert(uint64_t s0, uint64_t s1) { return Convert(Convert(s0), s1); }
		static constexpr inline uint32_t Convert(uint64_t s0, uint64_t s1, uint64_t s2) { return Convert(Convert(s0, s1), s2); }
		static constexpr inline uint32_t Convert(uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3) { return Convert(Convert(s0, s1, s2), s3); }
		static constexpr inline uint32_t Convert(uint32_t keys) { return keys; }
#else
		static constexpr inline uint32_t Convert(uint32_t v, char s) { return s == '-' ? v : ((v ^ (uint32_t)Lower(s)) * _FNV_prime); }
		static constexpr inline uint32_t Convert(uint32_t v, const char* s) {return *s ? Convert(Convert(v, *s), s + 1) : v; }
		static constexpr inline uint32_t Convert(const char* s0) { return Convert(_FNV_offset_basis, s0); }
		static constexpr inline uint32_t Convert(const char* s0, const char* s1) { return Convert(Convert(s0), s1); }
		static constexpr inline uint32_t Convert(const char* s0, const char* s1, const char* s2) { return Convert(Convert(s0, s1), s2); }
		static constexpr inline uint32_t Convert(const char* s0, const char* s1, const char* s2, const char* s3) { return Convert(Convert(s0, s1, s2), s3); }
		static constexpr inline uint32_t Convert(uint32_t v, uint64_t s) { return (s & 0xff) ? Convert(Convert(v, (char)(s & 0xff)), static_cast<uint64_t>(s >> 8)) : v; }
		static constexpr inline uint32_t Convert(uint64_t s0) { return Convert(_FNV_offset_basis, s0); }
		static constexpr inline uint32_t Convert(uint64_t s0, uint64_t s1) { return Convert(Convert(s0), s1); }
		static constexpr inline uint32_t Convert(uint64_t s0, uint64_t s1, uint64_t s2) { return Convert(Convert(s0, s1), s2); }
		static constexpr inline uint32_t Convert(uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3) { return Convert(Convert(s0, s1, s2), s3); }
		static constexpr inline uint32_t Convert(uint32_t keys) { return keys; }
#endif
	public:
		bool Load(uint32_t id, void* para = 0)const;
		bool Unload(uint32_t id, void* para = 0, bool invoke = false)const;
	public:
		int Finds(const char* type, item_t* items[], int max)const;
		int Finds(uint32_t type, item_t* items[], int max)const;
		item_t* Find(uint32_t id)const;
		Object* Create(item_t*)const;
		Object* Create(uint32_t id)const;
		item_t* Register(const char* name, FuncCreate creater, void* param = 0);
		item_t* ReRegister(const char* name, uint32_t src);
		bool UnRegister(const char* name);
	public:
		template<class T>
		item_t* Register(const char* name, void* para = 0) {return Register(name, []() {return (Object*)new T(); }, para);}
		template<class X>
		item_t* ReRegister(const char* name, X src) { return ReRegister(name, Convert(src)); }
	public:
		template<class...Xs>
		item_t* Find(Xs...id)const {return Find(Convert(id...));}
		template<class T, class...Xs>
		T* Create(Xs...id)const { return static_cast<T*>(Create(Convert(id...))); }
	public:
		mutable SimpleTableC<ILoader*, 32> _loaders;
	};

	template<int C>
	class TReflection: public Reflection {
	private:
		item_t _items[C];
	public:
		TReflection() :Reflection{ _items, C } {}
	};
}

