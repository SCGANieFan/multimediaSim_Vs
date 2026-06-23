#pragma once
#include <stdint.h>
#include "SmfPortingThread.h"
#include "mutex.h"

namespace smf {
	class ThreadList {
	public:
		class Item_t:public ThreadItem_t {
		public:
			void Release() { _name = 0; }
			const char* Name()const { return (const char*)&_name; }
			uint32_t Usage()const;
			void Update();
			char* Print(char* ptr, char* end)const;
		};
	public:
		ThreadList() {}
		ThreadList(Item_t* items, int cnt) { Init(items, cnt); }
		void Init(Item_t* items, int cnt);
	private:
		Item_t* _items = 0;
		int _cnt = 0;
	public:
		Item_t* Hold(const char* name);
		//void Release(const char* name);
		//
		//Item_t* Find(const char* name)const;
		uint32_t Idle()const;
		uint32_t Usage()const;
		uint32_t Workloads()const;
		char* Print(char* ptr, char* end)const;
		void Update();
		mutex _mtx;
	public:
		static ThreadList& List();
	};

	template<int C>
	class TThreadList :public ThreadList {
	private:
		Item_t _items[C];
	public:
		TThreadList() :ThreadList{ _items,C } {}
	};
}
