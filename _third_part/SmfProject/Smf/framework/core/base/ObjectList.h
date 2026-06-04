#pragma once
#include <stdint.h>
namespace smf {
	class Object;
	class ObjectList {
	public:
		~ObjectList();
		ObjectList();
		ObjectList(Object** objs, int cnt);
		friend class Object;
	protected:
		Object** _objs = 0;
		int _cnt = 0;
	public:
		void Init(Object** objs, int cnt);
		operator bool() const { return _objs; }
		int Count()const { return _cnt; }
		Object*& Item(uint32_t idx) { return _objs[idx]; }
		Object* Item(uint32_t idx) const { return (Object*)_objs[idx]; }
		Object*& operator[](uint32_t idx) { return _objs[idx]; }
		Object* operator[](uint32_t idx) const { return (Object*)_objs[idx]; }
		//
		Object** begin() const { return _objs; }
		Object** end() const { return _objs + _cnt; }
		Object** begin() { return _objs; }
		Object** end() { return _objs + _cnt; }
		//
		bool Has(Object*obj)const { return Find(obj); }
		bool Has(uint64_t id)const { return Find(id); }
		bool Has(const char* name)const { return Find(name); }
		//
		Object** Find(bool(*func)(Object*obj,void* priv),void* priv)const;
		Object** Find(Object*)const;
		Object** Find(uint64_t)const;
		Object** Find(const char*)const;
		//
		void Foreach(void(*func)(Object* obj, void* priv), void* priv)const;
		void rForeach(void(*func)(Object* obj, void* priv), void* priv)const;
		//
		bool Add(Object*,Object*parent,bool changeParent = true);
		bool Remove(Object*);
		bool Remove(uint64_t);
		bool Remove(const char*);
		//
		bool Replace(Object* src, Object* dst);
		//
		void Clear(bool changeParent = true);
		//
		void Delete(Object* obj);
		void Delete(const char* name);
		void Delete(uint64_t id);
	};
}

