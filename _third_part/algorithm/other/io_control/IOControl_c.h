#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LOG(fmt,...) printf("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)

#if 0
class IOControlerListInner_c {
public:
	IOControlerListInner_c(IOControlerListInner_c** val, uint32_t N);
	~IOControlerListInner_c();
public:
	bool Add(uint32_t id);
	bool Remove(uint32_t id);
	bool AddList(IOControlerListInner_c* val);
	bool RemoveList(uint32_t id);
public:
	bool IsListEqual(IOControlerListInner_c* list);
	void Print();
public:
	uint32_t _id;
	IOControlerListInner_c** _val;
	uint32_t _N;
	uint32_t _num;
};


template <int N>
class IOControlerList_c : public IOControlerListInner_c {
public:
	IOControlerList_c() :IOControlerListInner_c(_list, N) {}
	~IOControlerList_c() {}
public:
	IOControlerListInner_c* _list[N];
};
#else

template<class T>
class List
{
public:
	List(T* list, uint32_t n) {
		_list = list;
		_N = n;
		for (uint32_t n = 0; n < _N; n++) {
			_list[n] = 0;
		}
	}
	~List() {}
	T*& GetList() { return _list; }
protected:
	T* _list;
	uint32_t _N;
};

class IOControlerListInner_c :public List<IOControlerListInner_c*> {
public:
	IOControlerListInner_c(IOControlerListInner_c** val, uint32_t N);
	~IOControlerListInner_c();
public:
	bool Add(uint32_t id);
	bool Remove(uint32_t id);
	bool AddList(IOControlerListInner_c* val);
	bool RemoveList(uint32_t id);
public:
	bool IsListEqual(IOControlerListInner_c* list);
	uint32_t GetIdFromList(uint8_t idx);
	void Print();
public:
	uint32_t _id;
	uint32_t _num;
};


template <int N>
class IOControlerList_c : public IOControlerListInner_c {
public:
	IOControlerList_c() :IOControlerListInner_c(_array, N) {}
	~IOControlerList_c() {}
public:
	IOControlerListInner_c* _array[N];
};
#endif

using IOControlerInputItem_c = IOControlerList_c<8>;
using IOControlerOutputItem_c = IOControlerList_c<8>;


class IOControlerStrategy_c {
public:
	IOControlerStrategy_c() {}
	~IOControlerStrategy_c() {}
public:
	bool AddIn(uint32_t id, uint32_t oId);
	bool AddOut(uint32_t id);
	bool RemoveIn(uint32_t id, uint32_t oId);
	bool RemoveOut(uint32_t id);
private:

};

class IOControler_c
{
public:
	class Input_c;
	class Output_c;
	class IOSets_c;
	class IOMap_c;

	class Input_c {
	public:
		Input_c() {}
		~Input_c() {}
	public:
		bool Print();
	public:
		IOControlerInputItem_c _item[8];
	};

	class Output_c {
	public:
		Output_c() {}
		~Output_c() {}
	public:
		bool Print();
	public:
		IOControlerOutputItem_c _item[8];
	};

	class IOSets_c {
	public:
		class Item {
		public:
			Item() {}
			~Item() {}
		public:
			uint32_t _idIn;
			uint32_t _idOut;
		};
	public:
		IOSets_c() { Reset(); }
		~IOSets_c() {}
	public:
		bool Reset();
		bool Add(uint32_t idIn, uint32_t idOut);
		bool Remove(uint32_t idIn, uint32_t idOut);
		void Print();
	public:
		Item _items[8];
	};

	class IOMap_c {
#if 1
	public:
		class IOItem_c {
			static constexpr size_t ITEM_MAX = 8U;
		public:
			IOItem_c() {}
			~IOItem_c() {}
		private:
			template<class T>
			bool AppendOne(T newId, T(&arr)[ITEM_MAX]) {
				for (T& val : arr) {
					if (!val) {
						val = newId;
						return true;
					}
				}
				return false;
			}
		public:
			bool AppendInput(uint32_t id);
			bool AppendOutput(uint32_t id);
			bool OutEqual(IOItem_c& item);
			bool Equal(IOItem_c& item);
			bool Reset(uint8_t v);
		public:
			//Output_c *_output[8];
			uint8_t _oIndex[8];
			uint8_t _num;
		};
#endif
	public:
		static constexpr uint8_t INVALID_INDEX = 0xff;
	public:
		IOMap_c() {
			_output = 0;
			for (IOItem_c& item : _ioItems) item.Reset(INVALID_INDEX);
		}
		~IOMap_c() {}
	public:
		bool UpdateByScanIo(Input_c* input, Output_c* output, IOSets_c* sets);
		void Print();
	public:
		bool OutputHasChanged(uint32_t oid);
		bool OutputHasRemoved(uint32_t oid);
		bool OutputHasAdded(uint32_t oid);
	public:
		Output_c* _output;
		IOItem_c  _ioItems[8];
		//uint8_t _oIndex[8][8];
	};



public:
	IOControler_c() {
		_ioMap._output = &_output;
	}
	~IOControler_c() {}
public:
	bool AddInputDynamic(uint32_t id);
	bool RemoveInputDynamic(uint32_t id);
	bool AddOutputDynamic(uint32_t id);
	bool RemoveOutputDynamic(uint32_t id);
	bool AddIOSetsDynamic(uint32_t idIn, uint32_t idOut);
	bool RemoveIOSetsDynamic(uint32_t idIn, uint32_t idOut);
	bool GetIomapDynamic(IOMap_c*& map, uint32_t& num);
private:
	Input_c _input;
	Output_c _output;
	IOSets_c _ioSets;
	IOMap_c _ioMap;
	IOControlerStrategy_c *_ioStrategy;

};


