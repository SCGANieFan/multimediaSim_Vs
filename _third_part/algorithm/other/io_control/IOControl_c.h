#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



template<class T>
class List_c
{
public:
	List_c(T* list, uint32_t N) {
		_list = list;
		_N = N;
		for (uint32_t n = 0; n < _N; n++) {
			_list[n] = 0;
		}
	}
	~List_c() {}
public:
	T* _list;
	uint32_t _N;
};

class IOControlerListInner_c :public List_c<IOControlerListInner_c*> {
public:
	IOControlerListInner_c(IOControlerListInner_c** list,uint32_t n);
	~IOControlerListInner_c();
public:
	bool Add(uint32_t id);
	bool Remove(uint32_t id);
	bool AddList(IOControlerListInner_c* val);
	bool RemoveList(uint32_t id);
public:
	bool IsListEqual(IOControlerListInner_c* list);
	uint32_t GetIdFromList(uint8_t idx);
	IOControlerListInner_c* GetListFromId(uint32_t id);
	bool Has(uint32_t id);
	int32_t IndexFromId(uint32_t id);
	void Print();
public:
	uint32_t _id;
	uint32_t _num;
};

template <uint32_t N>
class IOControlerList_c :public IOControlerListInner_c {
public:
	IOControlerList_c() : IOControlerListInner_c(_array, N) {}
	~IOControlerList_c() {}
public:
	IOControlerListInner_c* _array[N];
};



using Input_c = IOControlerList_c<8>;
using Output_c = IOControlerList_c<8>;

class Inputs_c;
class Outputs_c;
class IOSets_c;
class IOMap_c;

class InOutputs_c {
public:
	InOutputs_c(IOControlerListInner_c* list, uint32_t  n) {
		_list = list;
		_N = n;
	}
	~InOutputs_c() {}
public:
	bool HasId(uint32_t id);
	int32_t IndexFromId(uint32_t id);
protected:
	IOControlerListInner_c* GetListFromId(uint32_t id);
public:
	IOControlerListInner_c* _list;
	uint32_t  _N;
};


class Inputs_c :public InOutputs_c {
public:
	Inputs_c() :InOutputs_c(_input, sizeof(_input) / sizeof(_input[0])) {}
	~Inputs_c() {}
public:
	Input_c* GetInputFromId(uint32_t id) {
		return (Input_c*)InOutputs_c::GetListFromId(id);
	}
	bool Print();
public:
	Input_c _input[8];
};


class Outputs_c :public InOutputs_c {
public:
	Outputs_c() :InOutputs_c(_output, sizeof(_output) / sizeof(_output[0])) {}
	~Outputs_c() {}
public:
	Output_c* GetOutputFromId(uint32_t id) {
		return (Output_c*)InOutputs_c::GetListFromId(id);
	}
	bool Print();
public:
	Output_c _output[8];
};


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
	bool GetIdoutFromIdin(uint32_t idIn, uint32_t& idOut);
	void Print();
public:
	Item _items[8];
};




class IOMap_c {
public:
	class IOItem_c {
	public:
		IOItem_c() {
			for (Output_c*& v : _output) v = 0;
		}
		~IOItem_c() {}
	public:
		bool Reset();
	public:
		Output_c* _output[8];
		uint8_t _outputNum = 0;
		uint8_t _numExit = 0;
		uint8_t _numExitThreshold = 0;
		uint8_t _numEntry = 0;
		uint8_t _numEntryThreshold = 0;
	};
	enum class IOMapCmd_c {
		IOMapCmdOutputHasChanged=1,
		IOMapCmdOutputHasRemoved,
		IOMapCmdOutputHasAdded,
		IOMapCmdOutputHasNull=0xff,
	};
	class Msg_c {
	public:
		Msg_c() {
			cmd = IOMapCmd_c::IOMapCmdOutputHasNull;
			id = 0;
		}
		~Msg_c() {}
	public:
		IOMapCmd_c cmd;
		uint32_t id;
	};
public:
	IOMap_c() {
		_outputs = 0;
	}
	~IOMap_c() {}
public:
	void Print();
public:
	bool OutputHasChangedClient(uint32_t oid) {
		if (!oid) return false;
		Msg_c msg;
		msg.cmd = IOMapCmd_c::IOMapCmdOutputHasChanged;
		msg.id = oid;
		return PushCmd(msg);
	}
	bool OutputHasRemovedClient(uint32_t oid) {
		if (!oid) return false;
		Msg_c msg;
		msg.cmd = IOMapCmd_c::IOMapCmdOutputHasRemoved;
		msg.id = oid;
		return PushCmd(msg);
	}
	bool OutputHasAddedClient(uint32_t oid) {
		if (!oid) return false;
		Msg_c msg;
		msg.cmd = IOMapCmd_c::IOMapCmdOutputHasAdded;
		msg.id = oid;
		return PushCmd(msg);
	}
public:
	bool OutputHasChanged(uint32_t oid);
	bool OutputHasRemoved(uint32_t oid);
	bool OutputHasAdded(uint32_t oid);
public:
	void update();
	bool PushCmd(Msg_c& msg) {
		if ((_msgWi - _msgRi + 1) > 8) {
			return false;
		}
		_msgs[_msgWi % 8] = msg;
		++_msgWi;
		return true;
	}
	bool PopCmd(Msg_c& msg) {
		if (_msgRi >= _msgWi) return false;
		msg = _msgs[_msgRi % 8];
		++_msgRi;
		return true;
	}
public:
	Outputs_c* _outputs;
	IOItem_c  _ioItems[8];
	Msg_c _msgs[8];
	uint32_t _msgWi = 0;
	uint32_t _msgRi = 0;
};






class IOControler_c
{
public:
	IOControler_c() {
		_ioMap._outputs = &_outputs;
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
	Inputs_c _inputs;
	Outputs_c _outputs;
	IOSets_c _ioSets;
	IOMap_c _ioMap;
	IOControlerStrategy_c *_ioStrategy;

};


