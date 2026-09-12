#include "IOControl_c.h"

#define LOG(fmt,...) printf("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)


IOControlerListInner_c::IOControlerListInner_c(IOControlerListInner_c** list, uint32_t n) :List_c(list, n) {
	_id = 0;
	_num = 0;
}
IOControlerListInner_c::~IOControlerListInner_c() {}
bool IOControlerListInner_c::Add(uint32_t id) {
	if (_id) return false;
	if (!id) return false;
	_id = id;
	LOG("%s", (const char*)&id);
}
bool IOControlerListInner_c::Remove(uint32_t id) {
	if (!id) return false;
	if (id != _id) return false;
	for (uint32_t n = 0; n < _N; n++) {
		if (!_list[n]) continue;
		if (!_list[n]->_id) continue;
		_list[n]->RemoveList(_id);
		LOG("%s !<- %s", (const char*)&_id, (const char*)&_list[n]->_id);
		_list[n] = 0;
		--_num;
	}
	LOG("%s", (const char*)&_id);
	_id = 0;
	return true;
}
bool IOControlerListInner_c::AddList(IOControlerListInner_c* val) {
	if (!val) return false;
	bool has = false;
	for (uint32_t n = 0; n < _N; n++) {
		if (!_list[n]) continue;
		if (_list[n]->_id != val->_id) continue;
		has = true;
		break;
	}
	if (has) return false;
	bool success = false;
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]) continue;
		_list[n] = val;
		++_num;
		LOG("%s <- %s", (const char*)&_id, (const char*)&val->_id);
		success = true;
		break;
	}
	if (!success) return false;
	return true;
}
bool IOControlerListInner_c::RemoveList(uint32_t id) {
	if (!id) return false;
	bool successs = false;
	for (uint32_t n = 0; n < _N; n++) {
		if (!_list[n]) continue;
		if (_list[n]->_id != id) continue;
		LOG("%s !<- %s", (const char*)&_id, (const char*)&id);
		_list[n] = 0;
		--_num;
		successs = true;
		break;
	}
	if (!successs)return false;
	return true;
}

bool IOControlerListInner_c::IsListEqual(IOControlerListInner_c* list) {
	if (!list) return false;
	if (_num != list->_num) return false;
	for (uint32_t n = 0; n < _N; n++) {
		if (!_list[n]) continue;
		bool isEqual = false;
		for (uint32_t k = 0; k < list->_N; k++) {
			if (!list->_list[k]) continue;
			if (list->_list[k]->_id != _list[n]->_id) continue;
			isEqual = true;
			break;
		}
		if (!isEqual) return false;
	}
	return true;
}

uint32_t IOControlerListInner_c::GetIdFromList(uint8_t idx) {
	uint8_t idx0 = 0;
	for (uint8_t n = 0; n < _N; n++) {
		if (!_list[n]->_id) continue;
		if (idx0 == idx) return _list[n]->_id;
		++idx0;
	}
	return 0;
}

IOControlerListInner_c* IOControlerListInner_c::GetListFromId(uint32_t id) {
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]->_id != id) continue;
		return _list[n];
	}
	return 0;
}

bool IOControlerListInner_c::Has(uint32_t id) {
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]->_id != id) continue;
		return true;
	}
	return false;
}

int32_t IOControlerListInner_c::IndexFromId(uint32_t id) {
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]->_id != id) continue;
		return n;
	}
	return -1;
}

void IOControlerListInner_c::Print() {
	LOG("%s:", (const char*)&_id);
	for (uint32_t n = 0; n < _N; n++) {
		if (!_list[n]) continue;
		LOG("[%d]%s", n, (const char*)&_list[n]->_id);
	}
}

bool InOutputs_c::HasId(uint32_t id) {
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]._id != id) continue;
		return true;
	}
	return false;
}

int32_t InOutputs_c::IndexFromId(uint32_t id) {
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]._id != id) continue;
		return n;
	}
	return -1;
}

IOControlerListInner_c* InOutputs_c::GetListFromId(uint32_t id) {
	for (uint32_t n = 0; n < _N; n++) {
		if (_list[n]._id != id) continue;
		return &_list[n];
	}
	return 0;
}

bool Inputs_c::Print() {
	uint32_t n = 0;
	//LOG("[input][%u] %s", n, (const char*)&item._id);
	LOG("[input]");
	for (Input_c& input: _input) {
		if (!input._id) continue;
		input.Print();
		++n;
	}
	return true;
}


bool Outputs_c::Print() {
#if 1
	uint32_t n = 0;
	//LOG("[output][%u] %s", n, (const char*)&item._id);
	LOG("[output]");
	for (Output_c & output : _output) {
		if (!output._id) continue;
		output.Print();
		++n;
	}
	return true;
#endif
}



bool IOSets_c::Reset() {
	for (Item& item : _items) {
		item._idIn = 0;
		item._idOut = 0;
	}
	return true;
}

bool IOSets_c::Add(uint32_t idIn, uint32_t idOut) {
	if (!idIn || !idOut) return false;
	for (Item& item : _items) {
		if (item._idIn == idIn) return false;
	}
	for (Item& item : _items) {
		if (item._idIn) continue;
		item._idIn = idIn;
		item._idOut = idOut;
		LOG("[iosets] %s -> %s", (const char*)&idIn, (const char*)&idOut);
		return true;
	}
	return false;
}

bool IOSets_c::Remove(uint32_t idIn, uint32_t idOut) {
	if (!idIn || !idOut) return false;
	bool ret = false;
	for (Item& item : _items) {
		if (item._idIn == idIn) {
			if (idOut) {
				if (item._idOut == idOut) {
					LOG("[iosets] %s !-> %s", (const char*)&item._idIn, (const char*)&item._idOut);
					item._idIn = 0;
					item._idOut = 0;
					return true;
				}
				continue;
			}
			LOG("[iosets] %s !-> %s", (const char*)&item._idIn, (const char*)&item._idOut);
			item._idIn = 0;
			item._idOut = 0;
			ret = true;
		}
	}
	return ret;
}

bool IOSets_c::GetIdoutFromIdin(uint32_t idIn, uint32_t &idOut) {
	if (!idIn) return false;
	for (Item& item : _items) {
		if (item._idIn != idIn) continue;
		idOut = item._idOut;
		return true;
	}
	return false;
}


void IOSets_c::Print() {
	uint32_t n = 0;
	//LOG("IOSet_c:");
	for (Item& item : _items) {
		if (item._idIn) {
			LOG("[IOSet_c][%u]%s->%s", n++, (const char*)&item._idIn, (const char*)&item._idOut);
		}
	}
}

#if 0
bool IOMap_c::IOItem_c::Reset() {
	for (Output_c*& v : _output) v = 0;
	_outputNum = 0;
	return true;
}
#endif

void IOMap_c::Print() {
	LOG("[iomap]");
	uint8_t n = 0;
	for (IOItem_c& item : _ioItems) {
		if (!item._outputNum) continue;
		LOG("[%d], %d", n, item._outputNum);
		for (Output_c*& output : item._output) {
			if (!output) continue;
			LOG("%s", (const char*)&output->_id);
		}
		++n;
	}
}

bool IOMap_c::OutputHasChanged(uint32_t oid) {
	Output_c* outputChanged = 0;
	for (IOItem_c& item : _ioItems) {
		if (!item._outputNum) continue;
		for (Output_c*& output : item._output) {
			if (!output) continue;
			if (output->_id != oid) continue;
			outputChanged = output;
			output = 0;
			--item._outputNum;
			break;
		}
		if (outputChanged != 0) break;
	}
	if (!outputChanged) {
		LOG("!outputChanged");
		return false;
	}
	
	for (IOItem_c& item : _ioItems) {
		if (!item._outputNum) continue;
		bool isEqual = false;
		for (Output_c*& output : item._output) {
			if (!output) continue;
			isEqual = outputChanged->IsListEqual(output);
			break;
		}
		if (isEqual) {
			bool success = false;
			for (Output_c*& output : item._output) {
				if (output) continue;
				output = outputChanged;
				++item._outputNum;
				success = true;
				break;
			}
			if (!success) {
				LOG("!success");
			}
			return true;
		}
	}
	for (IOItem_c& item : _ioItems) {
		if (item._outputNum) continue;
		bool success = false;
		for (Output_c*& output: item._output) {
			if (output) continue;
			output = outputChanged;
			++item._outputNum;
			success = true;
			break;
		}
		if (!success) {
			LOG("!success");
		}
		return true;
	}
	return true;
}
bool IOMap_c::OutputHasRemoved(uint32_t oid) {
	for (IOItem_c& item : _ioItems) {
		if (!item._outputNum) continue;
		for (Output_c*& output: item._output) {
			if (!output) continue;
			//if (output->_id == oid) {
			if (!output->_id) {
				output = 0;
				--item._outputNum;
				return true;
			}
		}
	}
	return false;
}
bool IOMap_c::OutputHasAdded(uint32_t oid) {
	Output_c* outputFromOutputs = _outputs->GetOutputFromId(oid);
	if (!outputFromOutputs) {
		LOG("!output");
		return false;
	}
	for (IOItem_c& item : _ioItems) {
		if (!item._outputNum) continue;
		bool equal = false;
		for (Output_c*& output : item._output) {
			if (!output) continue;
			equal = outputFromOutputs->IsListEqual(output);
			break;
		}
		if (equal) {
			bool success = false;
			for (Output_c*& output : item._output) {
				if (output) continue;
				output = outputFromOutputs;
				++item._outputNum;
				success = true;
				break;
			}
			if (success) {
				return true;
			}
			LOG("!success");
			break;
		}
	}
	for (IOItem_c& item : _ioItems) {
		if (item._outputNum) continue;
		bool success = false;
		for (Output_c*& output : item._output) {
			if (output) continue;
			output = outputFromOutputs;
			++item._outputNum;
			success = true;
			break;
		}
		if (!success) {
			LOG("!success");
		}
		return true;
	}
	return false;
}

void IOMap_c::update() {
	Msg_c msg;
	while (PopCmd(msg)) {
		LOG("%s,%d", (const char*)&msg.id, msg.cmd);
		switch (msg.cmd)
		{
		case IOMapCmd_c::IOMapCmdOutputHasChanged:
			OutputHasChanged(msg.id);
			break;
		case IOMapCmd_c::IOMapCmdOutputHasRemoved:
			OutputHasRemoved(msg.id);
			break;
		case IOMapCmd_c::IOMapCmdOutputHasAdded:
			OutputHasAdded(msg.id);
			break;
		default:
			break;
		}
	}
}

bool IOControler_c::AddInputDynamic(uint32_t id){
	if (!id) return false;
	if (_inputs.HasId(id)) return true;
	uint32_t idOutFromIOSet = 0;
	if (!_ioSets.GetIdoutFromIdin(id, idOutFromIOSet)) {
		idOutFromIOSet = 0;
	}
	if (!idOutFromIOSet) {
		for(Input_c& input: _inputs._input) {
			if (input._id) continue;
			input.Add(id);
			for (Output_c& output: _outputs._output) {
				if (!output._id) continue;
				input.AddList(&output);
				output.AddList(&input);
			}
			break;
		}
	}
	else {
		for (Input_c& input: _inputs._input) {
			if (input._id) continue;
			input.Add(id);
			for (Output_c& output : _outputs._output) {
				//if (!oItem._id) continue;
				if (output._id != idOutFromIOSet) continue;
				input.AddList(&output);
				output.AddList(&input);
				_ioMap.OutputHasChangedClient(output._id);
				break;
			}
			break;
		}
	}
	return true;
}
bool IOControler_c::RemoveInputDynamic(uint32_t id){
	if (!id) return false;
	if (!_inputs.HasId(id)) return false;
	for (Input_c& input: _inputs._input) {
		if (input._id != id) continue;
		bool notifyIOMap = false;
		uint32_t removeOid = 0;
		if (input._num == 1) {
			removeOid = input.GetIdFromList(0);
#if 0
			for (IOControlerListInner_c*& list : iItem._list) {
				if (!list->_id) continue;
				removeOid = list->_id;
				break;
			}
#endif
			notifyIOMap = true;
		}
		input.Remove(input._id);
		if (notifyIOMap) {
			_ioMap.OutputHasChangedClient(removeOid);
		}
		break;
	}
	return true;
}
bool IOControler_c::AddOutputDynamic(uint32_t id){
	if (!id) return false;
	if (_outputs.HasId(id)) return false;
	
	bool success = false;
	for (Output_c& output : _outputs._output) {
		if (output._id)continue;
		output.Add(id);
		for (Input_c& input : _inputs._input) {
			if (!input._id) continue;
			uint32_t idOut = 0;
			if (!_ioSets.GetIdoutFromIdin(input._id, idOut)) {
				idOut = 0;
			}
			if (!idOut || idOut == id) {
				output.AddList(&input);
				input.AddList(&output);
			}
		}
		success = true;
		break;
	}
	if (!success) return false;
	_ioMap.OutputHasAddedClient(id);
	return true;
}
bool IOControler_c::RemoveOutputDynamic(uint32_t id){
	if (!id) return false;
	if (!_outputs.HasId(id)) return false;
	bool success = false;
	for (Output_c& output: _outputs._output) {
		if (output._id != id) continue;
		//oItem.RemoveList(id);
		output.Remove(id);
		_ioMap.OutputHasRemovedClient(id);
		success = true;
		break;
	}
	if (!success) return false;
	return true;
}

bool IOControler_c::AddIOSetsDynamic(uint32_t idIn, uint32_t idOut){
	uint32_t idOutFromSets = 0;
	bool has = _ioSets.GetIdoutFromIdin(idIn, idOutFromSets);
	if (has)return false;
	_ioSets.Add(idIn, idOut);
	Input_c* inputSets = _inputs.GetInputFromId(idIn);
	if (!inputSets) return true;
	if (!idOut) {
		for (Output_c& output: _outputs._output) {
			if (!output._id) continue;
			output.AddList(inputSets);
			inputSets->AddList(&output);
		}
	}
	else {
		for (Output_c& output: _outputs._output) {
			if (!output._id) continue;
			if (output._id != idOut){
				output.RemoveList(inputSets->_id);
				inputSets->RemoveList(output._id);
				_ioMap.OutputHasChangedClient(output._id);
			}
			else {
				output.AddList(inputSets);
				inputSets->AddList(&output);
				_ioMap.OutputHasChangedClient(output._id);
			}
		}
	}
	return true;
}
bool IOControler_c::RemoveIOSetsDynamic(uint32_t idIn, uint32_t idOut) {
	uint32_t idOutFromSets = 0;
	bool has = _ioSets.GetIdoutFromIdin(idIn, idOutFromSets);
	if (!has) return false;

	Input_c* inputFromSet = _inputs.GetInputFromId(idIn);
	if (!inputFromSet) return true;
	for (Output_c& output: _outputs._output) {
		if (!output._id) continue;
		output.AddList(inputFromSet);
		inputFromSet->AddList(&output);
		_ioMap.OutputHasChangedClient(output._id);
	}
	_ioSets.Remove(idIn, idOut);
	return true;
}
bool IOControler_c::GetIomapDynamic(IOMap_c*& map, uint32_t& num) {
	_ioSets.Print();
	//_input.Print();
	_outputs.Print();
	_ioMap.update();
	_ioMap.Print();
	return true;
}

