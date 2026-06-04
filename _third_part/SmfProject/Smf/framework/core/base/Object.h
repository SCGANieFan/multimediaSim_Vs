#pragma once
#include "smf_common.h"
#include "smf_error.h"
#include "smf_debug.h"
#include "Object.def.h"
#include "ObjectList.h"
#include "ObjectStatic.h"
#include "ObjectCreater.h"
#include "SmfBase.h"
#include "SmfDef.h"
#include "SimpleTable.h"
#include "Flags.h"
namespace smf {
	class Object 
		:public api::SmfBase
		,public ObjectCreater
	{
	public:
		Object();
		virtual ~Object();
	private:
		friend class Reflection;
		friend class ObjectList;
	protected:
		virtual void initialize(Reflection::item_t*);
	private:
		Object* _parent = 0;
	protected:
		uint64_t _id = 0;
		mutable smf_error_t _error{ 0 };
		mutable Flags32 _flags;
		mutable Flags32 _flagsExt;
		cb_message_t _cbMsg = 0;
		void* _cbMsgPriv = 0;
		void* _tags = 0;
		ObjectList _children;
	private:
		void parentChange(Object* parent);
	public:///name
		void Name(const char* name);
		const char* Name() const;
		void ID(uint64_t);
		uint64_t ID() const;
		bool Equal(const char* name)const;
		bool Equal(uint64_t id)const;
		bool SetProperties(smf_keys_value_t* pair);
		bool SetProperties(const char* serial, void* params = 0);
		void* Tags() const { return (void*)_tags; }
		void Tags(void* tags) { _tags = tags; }
	public:
		Object* Parent()const { return _parent; }
		void Parent(Object*);
	public:
		Object* Child(const char* name, const char* end = 0) const;
		Object* Child(uint64_t id) const;
		Object* Find(const char* name, const char* end = 0) const { return Child(name, end); }
		Object* Find(uint64_t id) const { return Child(id); }
	public://children
		const ObjectList& Children() const { return _children; }
		ObjectList& Children() { return _children; }
		int Count()const { return _children.Count(); }
		bool Has(Object* obj) const { return _children.Has(obj); }
		void Clear() { _children.Clear(); }
		bool Add(Object* obj) { return _children.Add(obj,this); }
		bool Remove(Object*obj) { return _children.Remove(obj); }
		bool Remove(const char* name) { return _children.Remove(name); }
		bool Remove(uint64_t id) { return _children.Remove(id); }
		void Delete (Object* obj) { delete obj; }
		void Delete(const char* name) { _children.Delete(name); }
		void Delete(uint64_t id) { _children.Delete(id); }
		bool Replace(Object*src,Object*dst) { return _children.Replace(src,dst); }
		Object** begin() const { return _children.begin(); }
		Object** end() const { return _children.end(); }
		Object** begin() { return _children.begin(); }
		Object** end() { return _children.end(); }
	public:
		bool Add(const char* script, void* params = 0);
		bool SelectParamPack(const char* keys, void* params = 0);
		bool SelectParamPack(uint32_t keys, void* params = 0);
	public:
		using ParamTable = StringTable;
		virtual ParamTable* GetParamTable() const;
	public:
		char* Print(char* ptr, char* end) const;
		void Print() const;
	protected://event
		virtual void processEvent(uint32_t evt, void* para);
		void onEvent(uint32_t evt, void* para = 0);
		void onEvent(const char* evt, void* para = 0);
	protected://message
		virtual bool receiveMessage(smf_message_t&);
		virtual bool processMessage(smf_message_t&);
	public:
		using ObjectCreater::Register;
		bool Register(cb_message_t func, void* priv = 0);
		bool SendMessage(smf_message_t&);
		bool SendMessage(smf_message_t&, uint64_t id, smf_direction_e dir, uint32_t para0 = 0, uint32_t para1 = 0, uint32_t para2 = 0, uint32_t para3 = 0);
		bool SendMessage(smf_message_t&, const char* id, smf_direction_e dir, uint32_t para0 = 0, uint32_t para1 = 0, uint32_t para2 = 0, uint32_t para3 = 0);
		bool SendMessage(uint64_t msg, smf_direction_e dir, uint32_t para0 = 0, uint32_t para1 = 0, uint32_t para2 = 0, uint32_t para3 = 0);
		bool SendMessage(const char* msg, smf_direction_e dir, uint32_t para0 = 0, uint32_t para1 = 0, uint32_t para2 = 0, uint32_t para3 = 0);
	protected://children
		virtual Object* child(uint64_t id) const;
		virtual char* print(char* ptr, char* end) const;
	protected:///params Set/Get
		virtual bool set(uint32_t key, void* val);
		virtual bool get(uint32_t key, void* val) const;
		virtual bool run(void* params);
	public:
		bool Run(void* params) { return run(params); }
		bool Set(uint32_t key, void* val);
		bool Get(uint32_t key, void* val) const;
		bool Set(const char* key, void* val);
		bool Get(const char* key, void* val) const ;
		template<class T>inline
		bool Set(uint32_t key, T val) { return Set(key, (void*)(uintptr_t)val); }
		template<class T>inline
		bool Set(const char* key, T val) { return Set(key, (void*)(uintptr_t)val); }
	protected:///error
		void dbgErrProcess(const char* file, unsigned line, unsigned error)const;
	public:
		const smf_error_t& Error()const { return _error; }
		smf_error_t& Error(){ return _error; }
		bool IsError() const { return _flags.Check(IS_Error); }
		void SetError(unsigned error, const char* file = 0, unsigned line = 0) { dbgErrProcess(file,line,error); }
		void Error(const smf_error_t&err){ _error = err; }
		void CleanError()const;
	public:///flags
		const Flags32& Flags() const { return _flags; }
		Flags32& Flags() { return _flags; }
		const Flags32& FlagsExt() const { return _flagsExt; }
		Flags32& FlagsExt() { return _flagsExt; }
	public:
		static Object& Root();
	public:
		template<int C, class Base = Object, class XObject = Object>
		class List :public Base {
		public:
			List(const char* name = 0) {
				Base::Flags().Set(IS_List);
				if (name)Base::Name(name);
				for (auto& obj : _list) { obj = 0; }
				Base::_children.Init((Object**)_list, C);
			}
			virtual ~List() {
				Base::_children.Clear();
			}
		protected:
			XObject* _list[C];
		};
	};
}

