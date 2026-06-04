#include "ObjectList.h"
#include "Object.h"
#include "SmfFCC.h"
#include "smf_debug.h"
using namespace smf;
//
ObjectList::ObjectList() {}
ObjectList::~ObjectList() { Clear(); }
ObjectList::ObjectList(Object** objs, int cnt)
	:_objs(objs)
	,_cnt(cnt)
{
}
void ObjectList::Init(Object** objs, int cnt) {
	_objs = objs;
	_cnt = cnt;
}
//Object** ObjectList::Find(const std::function<bool(Object*)>& func)const {
//	for (int i = 0; i < _cnt; i++) {
//		if (func(_objs[i])) {
//			return &_objs[i];
//		}
//	}
//	return 0;
//}
Object** ObjectList::Find(bool(*func)(Object* obj, void* priv), void* priv)const {
	for (int i = 0; i < _cnt; i++) {
		if (func(_objs[i], priv)) {
			return &_objs[i];
		}
	}
	return 0;
}
Object** ObjectList::Find(Object* obj)const {
	for (int i = 0; i < _cnt; i++) {
		if (_objs[i]==obj) {
			return &_objs[i];
		}
	}
	return 0;
}
Object** ObjectList::Find(uint64_t id)const {
	for (int i = 0; i < _cnt; i++) {
		if (_objs[i] && _objs[i]->Equal(id)) {
			return &_objs[i];
		}
	}
	return 0;
}
Object** ObjectList::Find(const char* name)const{
	return Find(fcc64(name));
}

//void ObjectList::Foreach(const std::function<void(Object*)>& func)const {
//	for (int i = 0; i < _cnt; i++) {
//		if (_objs[i]) {
//			func(_objs[i]);
//		}
//	}
//}
//void ObjectList::rForeach(const std::function<void(Object*)>& func)const {
//	for (int i = _cnt - 1; i >= 0; i++) {
//		if (_objs[i]) {
//			func(_objs[i]);
//		}
//	}
//}
void ObjectList::Foreach(void(*func)(Object* obj, void* priv), void* priv) const {
	for (int i = 0; i < _cnt; i++) {
		if (_objs[i]) {
			func(_objs[i], priv);
		}
	}
}
void ObjectList::rForeach(void(*func)(Object* obj, void* priv), void* priv) const {
	for (int i = _cnt-1; i >= 0; i++) {
		if (_objs[i]) {
			func(_objs[i],priv);
		}
	}
}

bool ObjectList::Add(Object*obj, Object* parent, bool changeParent) {
	if (obj->_parent == parent) return true;
	auto pobj = Find((Object*)0);
	if (pobj) {
		*pobj = obj;
		if (changeParent) {
			if (obj->_parent)obj->_parent->Remove(obj);
			obj->parentChange(parent);
		}
	}
	return (bool)pobj;
	//for (int i = 0; i < _cnt; i++) {
	//	if (!_objs[i]) {
	//		_objs[i] = obj;
	//		if (obj->_parent)obj->_parent->Remove(obj);
	//		obj->_parent = parent;
	//		return true;
	//	}
	//}
	//return false;
}
bool ObjectList::Remove(Object*obj) {
	returnIf(false, !obj);
	auto pobj = Find(obj);
	if (pobj) {
		(*pobj)->parentChange(0);
		(*pobj) = 0;
	}
	return (bool)pobj;
	//for (int i = 0; i < _cnt; i++) {
	//	if (_objs[i] == obj) {
	//		_objs[i]->_parent = 0;
	//		_objs[i] = 0;
	//		return true;
	//	}
	//}
	//return false;
}
bool ObjectList::Remove(uint64_t id) {
	auto pobj = Find(id);
	if (pobj) {
		(*pobj)->parentChange(0);
		(*pobj) = 0;
	}
	return (bool)pobj;
	//for (int i = 0; i < _cnt; i++) {
	//	if (_objs[i] && _objs[i]->Equal(id)) {
	//		_objs[i]->_parent = 0;
	//		_objs[i] = 0;
	//		return true;
	//	}
	//}
	return false;
}
bool ObjectList::Remove(const char* name) {
	return Remove(fcc64(name));
}
bool ObjectList::Replace(Object* src, Object* dst) {
	for (int i = 0; i < _cnt; i++) {
		if (_objs[i] == src) {
			if (dst->Parent()) {
				dst->Parent()->Children().Remove(dst);
			}
			_objs[i] = dst;
			dst->parentChange(src->_parent);
			src->parentChange(0);
			return true;
		}
	}
	return false;
}

void ObjectList::Delete(Object* obj) {
	Delete(obj->ID());
}
void ObjectList::Delete(const char* name) {
	Delete(fcc64(name));
}
void ObjectList::Delete(uint64_t id) {
	auto pobj = Find(id);
	if (pobj) {
		auto obj = *pobj;
		*pobj = 0;
		obj->parentChange(0);
		delete obj;
	}
}

void ObjectList::Clear(bool changeParent) {
	for (int i = _cnt - 1; i >= 0; i--) {
		if (_objs[i]) {
			if (changeParent) {
				_objs[i]->parentChange(0);
				delete _objs[i];
			}
			_objs[i] = 0;
		}
	}
}