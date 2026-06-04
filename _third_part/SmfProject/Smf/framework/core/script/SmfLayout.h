#pragma once
#include "Object.h"
#include "mutex.h"
#include "string.hh"
#include "smf_debug.h"
namespace smf {
	class Layout{
	public:
		Layout(const char*script, bool is_url = false, bool no_copy = false, bool is_cpp = false);
	protected:
		string _buff;
		char* _script = 0;
	public:
		operator bool()const { return _script; }
		Object* Create(Object* parent = 0, uint32_t* params = 0);
		Object* Create(mutex&mtx, Object* parent = 0, uint32_t* params = 0);
		bool Parse(Object* parent = 0, uint32_t* params = 0);
		bool Parse(void*& vals, uint32_t* params = 0);
		bool Parse(const char*& keys, void*& vals, uint32_t* params = 0);
		typedef void(*CbPair)(void* priv, const char* keys, void* vals, char rst);
		bool Parse(CbPair cb, void* priv = 0, uint32_t* params = 0);
		int Parse(void** vals, char spliter, uint32_t* params = 0);
		bool Config(Object* obj, uint32_t* params = 0);
	public:
		template<class T = Object>
		static T* Create(const char* script, Object* parent = 0, void* params = 0) {
			return (T*)Layout(script).Create(parent, (uint32_t*)params);
		}
		template<class T = Object>
		static T* Create(mutex& mtx,const char* script, Object* parent = 0, void* params = 0) {
			return (T*)Layout(script).Create(mtx, parent, (uint32_t*)params);
		}
	public:
		template<class T>
		static bool Deserialize(T& t, const char* script, uint32_t* params = 0) {
			return Layout(script, false, true, false).Parse(&T::value_t::set, &t, params);
		}
	};
}
