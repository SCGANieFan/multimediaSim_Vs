#pragma once
#include "Reflection.h"
namespace smf {
	class ObjectCreater {
	public:
		static Reflection& Reflect();
		using FuncCreate = Reflection::FuncCreate;
	protected:
		Reflection::item_t* _class = 0;
	public:
		Reflection::item_t* Class() const { return _class; }
	public:
		template<class...Xs>
		static bool IsSupported(Xs...xs) {
			return (bool)Reflect().Find(xs...);
		}
	public:
		template<class T, class...Xs>
		static T* Create(Xs...xs) {
			return Reflect().Create<T>(xs...);
		}
	public:
		template<class T>
		static void Register(const char* type, void* para = 0){
			Reflect().Register<T>(type, para);
		}
		static void Register(const char* type, FuncCreate creater, void* para = 0) {
			Reflect().Register(type, creater, para);
		}
		template<class X>
		static void ReRegister(const char* type_dst	, X type_src) {
			Reflect().ReRegister(type_dst, type_src);
		}
		static void UnRegister(const char* type) {
			Reflect().UnRegister(type);
		}
	};
}

