#include "IOControl_c.h"


IOControlerListInner_c::IOControlerListInner_c(IOControlerListInner_c** val, uint32_t N):List(val, N) {
	_id = 0;
	//_N = N;
	//_val = val;
	_num = 0;
	//for (uint32_t n = 0; n < N; n++) {
	//	_val[n] = 0;
	//}
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
	IOControlerListInner_c**&list = GetList();
	for (uint32_t n = 0; n < _N; n++) {
		if (!list[n]) continue;
		if (!list[n]->_id) continue;
		list[n]->RemoveList(_id);
		LOG("%s !<- %s", (const char*)&_id, (const char*)&list[n]->_id);
		list[n] = 0;
		--_num;
	}
	LOG("%s", (const char*)&_id);
	_id = 0;
	return true;
}
bool IOControlerListInner_c::AddList(IOControlerListInner_c* val) {
	if (!val) return false;
	bool has = false;
	IOControlerListInner_c**& list = GetList();
	for (uint32_t n = 0; n < _N; n++) {
		if (!list[n]) continue;
		if (list[n]->_id != val->_id) continue;
		has = true;
		break;
	}
	if (has) return false;
	bool success = false;
	for (uint32_t n = 0; n < _N; n++) {
		if (list[n]) continue;
		list[n] = val;
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
	IOControlerListInner_c**& list = GetList();
	for (uint32_t n = 0; n < _N; n++) {
		if (!list[n]) continue;
		if (list[n]->_id != id) continue;
		LOG("%s !<- %s", (const char*)&_id, (const char*)&id);
		list[n] = 0;
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
		if (!GetList()[n]) continue;
		bool isEqual = false;
		for (uint32_t k = 0; k < list->_N; k++) {
			if (!list->GetList()[k]) continue;
			if (list->GetList()[k]->_id != GetList()[n]->_id) continue;
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
		if (!GetList()[n]->_id) continue;
		if (idx0 == idx) return GetList()[n]->_id;
		++idx0;
	}
	return 0;
}

void IOControlerListInner_c::Print() {
	LOG("%s:", (const char*)&_id);
	IOControlerListInner_c**& list = GetList();
	for (uint32_t n = 0; n < _N; n++) {
		if (!list[n]) continue;
		LOG("[%d]%s", n, (const char*)&list[n]->_id);
	}
}


bool IOControler_c::Input_c::Print() {
	uint32_t n = 0;
	//LOG("[input][%u] %s", n, (const char*)&item._id);
	LOG("[input]");
	for (IOControlerInputItem_c& item : _item) {
		if (!item._id) continue;
		item.Print();
		++n;
	}
	return true;
}


bool IOControler_c::Output_c::Print() {
#if 1
	uint32_t n = 0;
	//LOG("[output][%u] %s", n, (const char*)&item._id);
	LOG("[output]");
	for (IOControlerOutputItem_c& item : _item) {
		if (!item._id) continue;
		item.Print();
		++n;
	}
	return true;
#endif
}



bool IOControler_c::IOSets_c::Reset() {
	for (Item& item : _items) {
		item._idIn = 0;
		item._idOut = 0;
	}
	return true;
}

bool IOControler_c::IOSets_c::Add(uint32_t idIn, uint32_t idOut) {
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

bool IOControler_c::IOSets_c::Remove(uint32_t idIn, uint32_t idOut) {
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
void IOControler_c::IOSets_c::Print() {
	uint32_t n = 0;
	//LOG("IOSet_c:");
	for (Item& item : _items) {
		if (item._idIn) {
			LOG("[IOSet_c][%u]%s->%s", n++, (const char*)&item._idIn, (const char*)&item._idOut);
		}
	}
}

#if 0
bool IOControler_c::IOMap_c::IOItem_c::AppendInput(uint32_t id) {
	//return AppendOne(id, _inputId);
	return true;
}
bool IOControler_c::IOMap_c::IOItem_c::AppendOutput(uint32_t id) {
#if 0
	if (!_outputId) {
		_outputId = id;
		return true;
	}
#endif
	return false;
}
bool IOControler_c::IOMap_c::IOItem_c::OutEqual(IOItem_c& item) {
#if 0
	if (_outputId != item._outputId) return false;
#endif
	return true;
}
bool IOControler_c::IOMap_c::IOItem_c::Equal(IOItem_c& item) {
#if 0
	if (_outputId != item._outputId) return false;
	uint32_t num = 0;
	for (uint32_t& id : _inputId) {
		if (id) ++num;
	}
	uint32_t numIn = 0;
	for (uint32_t& id : item._inputId) {
		if (id) ++numIn;
	}
	if (num != numIn) return false;
	for (uint32_t& id : _inputId) {
		if (!id) continue;
		bool equal = false;
		for (uint32_t& idCmp : item._inputId) {
			if (!idCmp) continue;
			if (id != idCmp)continue;
			equal = true;
			break;
		}
		if (!equal) return false;
	}
	return true;
#endif
	return true;
}
#endif
bool IOControler_c::IOMap_c::IOItem_c::Reset(uint8_t v) {
	for (uint8_t& v0 : _oIndex) v0 = v;
	_num = 0;
	return true;
}

#if 0
bool IOControler_c::IOMap_c::Strategy_c::AppendNewIn(uint32_t id, uint32_t oId) {
	for (uint8_t n = 0; n < 8; n++) {
		uint8_t j = 0;
		for (j = 0; j < 8; j++) {
			if (!_items[n][j]) continue;
			break;
		}
		if (j == 8) {//empty
			_items[n][0] = &item;
			return true;
		}
	}
	return false;
	LOG("%s<-%s", (const char*)&oId, (const char*)&id);
	return true;
}
bool IOControler_c::IOMap_c::Strategy_c::AppendNewOut(uint32_t id) {
	LOG("%s", (const char*)&id);
	return true;
}
bool IOControler_c::IOMap_c::Strategy_c::RemoveIn(uint32_t id, uint32_t oId) {
	LOG("%s!<-%s", (const char*)&oId, (const char*)&id);
	return true;
}
bool IOControler_c::IOMap_c::Strategy_c::RemoveOut(uint32_t id) {
	LOG("%s", (const char*)&id);
	return true;
}

#endif

bool IOControler_c::IOMap_c::UpdateByScanIo(Input_c* input, Output_c* output, IOSets_c* sets) {
	//what changes?
#if 0
	//scan Superfluous output in _ioItems
	for (IOItem_c& ioItem : _ioItems) {
		if (!ioItem._outputId) continue;
		bool isEqual = false;
		for (Output_c::Item_c& oItem : output->_item) {
			if (!oItem._id) continue;
			if (ioItem._outputId != oItem._id) continue;
			isEqual = true;
			break;
		}
		if (!isEqual) {
#if 0
			for (uint32_t& id : ioItem._inputId) {
				if (!id) continue;
				_strategy.RemoveIn(id, ioItem._outputId);
				id = 0;
			}
#endif
			_strategy.RemoveOut(ioItem._outputId);
			ioItem._outputId = 0;
		}
	}

	//scan new output in output->_item
	for (Output_c::Item_c& oItem : output->_item) {
		if (!oItem._id) continue;
		bool isNewOutAppend = true;
		for (IOItem_c& ioItem : _ioItems) {
			if (!ioItem._outputId) continue;
			if (ioItem._outputId != oItem._id) continue;
			isNewOutAppend = false;
			break;
		}
		if (isNewOutAppend) {
			bool isNewAppendSuccess = false;
			for (IOItem_c& ioItem : _ioItems) {
				if (ioItem._outputId) continue;
				ioItem._outputId = oItem._id;
				isNewAppendSuccess = true;
				_strategy.AppendNewOut(ioItem._outputId);
				uint8_t idx = 0;
				for (Input_c::Item_c& iItem : input->_item) {
					if (!iItem._id) continue;
					//if (iItem._oId && iItem._oId != ioItem._outputId) continue;
					//ioItem._inputId[idx] = iItem._id;
					_strategy.AppendNewIn(iItem._id, ioItem._outputId);
					++idx;
				}
				break;
			}
		}
		else {
			for (IOItem_c& ioItem : _ioItems) {
				if (!ioItem._outputId) continue;
				if (ioItem._outputId != oItem._id) continue;
				//remove inpute
#if 0
				for (uint32_t& id : ioItem._inputId) {
					if (!id) continue;
					bool isEqual = false;
					for (Input_c::Item_c& iItem : input->_item) {
						if (id != iItem._id)continue;
						//if (iItem._oId && iItem._oId != ioItem._outputId)continue;
						isEqual = true;
						break;
					}
					if (!isEqual) {
						_strategy.RemoveIn(id, ioItem._outputId);
						id = 0;
					}
				}
#endif
				//append inpute
				for (Input_c::Item_c& iItem : input->_item) {
					if (!iItem._id) continue;
					//if (iItem._oId && (iItem._oId != ioItem._outputId)) continue;
					bool isEqual = false;
#if 0
					for (uint32_t& id : ioItem._inputId) {
						if (id != iItem._id) continue;
						isEqual = true;
						break;
					}
#endif
					if (!isEqual) {
						_strategy.AppendNewIn(iItem._id, ioItem._outputId);
						bool isAppendNewInSuccess = false;
#if 0
						for (uint32_t& id : ioItem._inputId) {
							if (id) continue;
							id = iItem._id;
							isAppendNewInSuccess = true;
							break;
						}
#endif
						if (!isAppendNewInSuccess)
							LOG("!isAppendNewInSuccess");
					}
				}
				break;
			}
		}
	}
#endif
	Print();
	return true;
}
void IOControler_c::IOMap_c::Print() {
	LOG("[iomap]");
	uint8_t n = 0;
	for (IOItem_c& item : _ioItems) {
		if (!item._num) continue;
		LOG("[%d], %d", n, item._num);
		for (uint8_t& index : item._oIndex) {
			if (index == INVALID_INDEX) continue;
			LOG("%s", (const char *)&_output->_item[index]._id);
		}
		++n;
	}
}

bool IOControler_c::IOMap_c::OutputHasChanged(uint32_t oid) {
	uint8_t idxChanged = INVALID_INDEX;
	for (IOItem_c& item : _ioItems) {
		if (!item._num) continue;
		for (uint8_t& index : item._oIndex) {
			if (index == INVALID_INDEX) continue;
			if (_output->_item[index]._id != oid) continue;
			idxChanged = index;
			index = INVALID_INDEX;
			--item._num;
			break;
			//LOG("%s", (const char*)&);
		}
		if (idxChanged != INVALID_INDEX) break;
	}
	if (idxChanged == INVALID_INDEX) {
		LOG("idxChanged == INVALID_INDEX");
		return false;
	}
	IOControlerOutputItem_c& oitmChanged = _output->_item[idxChanged];
	for (IOItem_c& item : _ioItems) {
		if (!item._num) continue;
		bool isEqual = false;
		for (uint8_t& index : item._oIndex) {
			if (index == INVALID_INDEX) continue;
			isEqual = oitmChanged.IsListEqual(&_output->_item[index]);
			break;
		}
		if (isEqual) {
			bool success = false;
			for (uint8_t& index : item._oIndex) {
				if (index != INVALID_INDEX) continue;
				index = idxChanged;
				++item._num;
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
		if (item._num) continue;
		bool success = false;
		for (uint8_t& index : item._oIndex) {
			if (index != INVALID_INDEX) continue;
			index = idxChanged;
			++item._num;
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
bool IOControler_c::IOMap_c::OutputHasRemoved(uint32_t oid) {
	for (IOItem_c& item : _ioItems) {
		if (!item._num) continue;
		for (uint8_t& index : item._oIndex) {
			if (index == INVALID_INDEX) continue;
			if (!_output->_item[index]._id) {
				index = INVALID_INDEX;
				--item._num;
			}
			//LOG("%s", (const char*)&);
		}
	}
	return true;
}
bool IOControler_c::IOMap_c::OutputHasAdded(uint32_t oid) {
	uint8_t idx = 0;
	bool has = false;
	for (IOControlerOutputItem_c& item : _output->_item) {
		if (item._id != oid) {
			++idx;
			continue;
		}
		has = true;
		break;
	}
	if (!has) {
		LOG("!has");
		return false;
	}
	IOControlerOutputItem_c& oitmChanged = _output->_item[idx];
	for (IOItem_c& item : _ioItems) {
		if (item._num) continue;
		bool success = false;
		for (uint8_t& index : item._oIndex) {
			if (index != INVALID_INDEX) continue;
			index = idx;
			++item._num;
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

bool IOControler_c::AddInputDynamic(uint32_t id){
	if (!id) return false;
	bool equal = false;
	for (IOControlerInputItem_c& iItem: _input._item) {
		if (iItem._id != id) continue;
		equal = true;
		break;
	}
	if (equal) return true;
	uint32_t idOutFromIOSet = 0;
	for (IOSets_c::Item& setItem : _ioSets._items) {
		if (id != setItem._idIn) continue;
		idOutFromIOSet = setItem._idOut;
		break;
	}
	if (!idOutFromIOSet) {
		for(IOControlerInputItem_c& iItem : _input._item) {
			if (iItem._id) continue;
			iItem.Add(id);
			for (IOControlerOutputItem_c& oItem : _output._item) {
				if (!oItem._id) continue;
				iItem.AddList(&oItem);
				oItem.AddList(&iItem);
			}
			break;
		}
	}
	else {
		for (IOControlerInputItem_c& iItem : _input._item) {
			if (iItem._id) continue;
			iItem.Add(id);
			for (IOControlerOutputItem_c& oItem : _output._item) {
				//if (!oItem._id) continue;
				if (oItem._id != idOutFromIOSet) continue;
				iItem.AddList(&oItem);
				oItem.AddList(&iItem);
				_ioMap.OutputHasChanged(oItem._id);
				break;
			}
			break;
		}
	}
	return true;
}
bool IOControler_c::RemoveInputDynamic(uint32_t id){
	if (!id) return false;
	for (IOControlerInputItem_c& iItem : _input._item) {
		if (iItem._id != id) continue;
		bool notifyIOMap = false;
		uint32_t removeOid = 0;
		if (iItem._num == 1) {
			removeOid = iItem.GetIdFromList(0);
#if 0
			for (IOControlerListInner_c*& list : iItem._list) {
				if (!list->_id) continue;
				removeOid = list->_id;
				break;
			}
#endif
			notifyIOMap = true;
		}
		iItem.Remove(iItem._id);
		if (notifyIOMap) {
			_ioMap.OutputHasChanged(removeOid);
		}
		break;
	}
	return true;
}
bool IOControler_c::AddOutputDynamic(uint32_t id){
	if (!id) return false;
	bool has = false;
	for (IOControlerOutputItem_c& oItem : _output._item) {
		if (oItem._id != id)continue;
		has = true;
		break;
	}
	if (has) return false;

	bool success = false;
	for (IOControlerOutputItem_c& oItem : _output._item) {
		if (oItem._id)continue;
		oItem.Add(id);
		for (IOControlerInputItem_c& iItem : _input._item) {
			if (!iItem._id) continue;
			uint32_t idOut = 0;
			for (IOSets_c::Item& setItem: _ioSets._items) {
				if (iItem._id != setItem._idIn) continue;
				idOut = setItem._idOut;
				break;
			}
			if (!idOut|| idOut == id) {
				oItem.AddList(&iItem);
				iItem.AddList(&oItem);
			}
		}
		success = true;
		break;
	}
	if (!success) return false;
	_ioMap.OutputHasAdded(id);
	return true;
}
bool IOControler_c::RemoveOutputDynamic(uint32_t id){
	if (!id) return false;
	bool success = false;
	for (IOControlerOutputItem_c& oItem : _output._item) {
		if (!oItem._id) continue;
		if (oItem._id != id) continue;
		//oItem.RemoveList(id);
#if 1
		oItem.Remove(id);
		_ioMap.OutputHasRemoved(id);
#else
		for (uint32_t n = 0; n < oItem._N; n++) {
			if (!oItem._val[n]) continue;
			if (!oItem._val[n]->_id) continue;
			oItem._val[n]->RemoveList(oItem._id);
			LOG("%s !<- %s", (const char*)&oItem._id, (const char*)&oItem._val[n]->_id);
			oItem._val[n] = 0;
			--oItem._num;
		}
		_ioMap.OutputHasChanged(id);
		LOG("%s", (const char*)&oItem._id);
		oItem._id = 0;
#endif
		success = true;
		break;
	}
	if (!success) return false;
	return true;
}
bool IOControler_c::AddIOSetsDynamic(uint32_t idIn, uint32_t idOut){
	bool has = false;
	for (IOSets_c::Item& setItem : _ioSets._items) {
		if (setItem._idIn != idIn) continue;
		//if (setItem._idOut != idOut) continue;
		has = true;
		break;
	}
	if (has)return false;
	_ioSets.Add(idIn, idOut);

	IOControlerInputItem_c* input = 0;
	for (IOControlerInputItem_c& iItem : _input._item) {
		if (iItem._id != idIn) continue;
		input = &iItem;
		break;
	}
	if (!input) return true;

	if (!idOut) {
		for (IOControlerOutputItem_c& oItem : _output._item) {
			if (!oItem._id) continue;
			oItem.AddList(input);
			input->AddList(&oItem);
		}
	}
	else {
		for (IOControlerOutputItem_c& oItem : _output._item) {
			if (!oItem._id) continue;
			if (oItem._id != idOut){
				oItem.RemoveList(input->_id);
				input->RemoveList(oItem._id);
				_ioMap.OutputHasChanged(oItem._id);
			}
			else {
				oItem.AddList(input);
				input->AddList(&oItem);
				_ioMap.OutputHasChanged(oItem._id);
			}
		}
	}
	return true;
}
bool IOControler_c::RemoveIOSetsDynamic(uint32_t idIn, uint32_t idOut) {
	bool has = false;
	for (IOSets_c::Item& setItem : _ioSets._items) {
		if (setItem._idIn != idIn) continue;
		has = true;
		break;
	}
	if (!has) return false;

	IOControlerInputItem_c* input = 0;
	for (IOControlerInputItem_c& iItem : _input._item) {
		if (iItem._id != idIn) continue;
		input = &iItem;
		break;
	}
	if (!input) return true;
	for (IOControlerOutputItem_c& oItem : _output._item) {
		if (!oItem._id) continue;
		oItem.AddList(input);
		input->AddList(&oItem);
		_ioMap.OutputHasChanged(oItem._id);
	}
	_ioSets.Remove(idIn, idOut);
	return true;
}
bool IOControler_c::GetIomapDynamic(IOMap_c*& map, uint32_t& num) {
	_ioSets.Print();
	//_input.Print();
	_output.Print();
	_ioMap.Print();
	return true;
}

