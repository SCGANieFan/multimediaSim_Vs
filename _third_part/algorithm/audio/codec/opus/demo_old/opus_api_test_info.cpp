#include <string.h>
#include <stdio.h>
#include "opus_api_test_info.h"


template<class K, class V>
struct KeyValue_t {
	K key;
	V val;
};

template<class T, uint32_t Num>
class Table_c{
public:
	Table_c() {
		_num = Num;
	}
	~Table_c() {}
public:
	T* Get(uint32_t index) { return index < _num ? &_tb[index] : 0; }
public:
	T _tb[Num];
	uint32_t _num;
};

template<class V, uint32_t Num>
class OpusTable_c : public Table_c<KeyValue_t<const char*, V>, Num> {
public:
	OpusTable_c() {}
	~OpusTable_c() {}
public:
	void Add(const char* key, V* val) {
		if (key) {
			for (uint32_t n = 0; n < this->_num; n++) {
				if (this->_tb[n].key && !strcmp(this->_tb[n].key, key)) {
					this->_tb[n].val = *val;
					this->_tb[n].key = key;
					return;
				}
			}
		}
		for (uint32_t n = 0; n < this->_num; n++) {
			if (!this->_tb[n].key) {
				this->_tb[n].val = *val;
				this->_tb[n].key = key;
				return;
			}
		}
	}
};

class TestInfoTable_c {
public:
	TestInfoTable_c() {
		encGetNum = 0;
		decGetNum = 0;
	}
	~TestInfoTable_c() {}
public:
	OpusTable_c<EncInfo_t, 10> encTable;
	uint32_t encGetNum;
	OpusTable_c<DecInfo_t, 10> decTable;
	uint32_t decGetNum;
};

TestInfoTable_c table;
#if 1
bool register_opus_test_info(void* info) {
	BaseInfo_t* baseInfo = (BaseInfo_t*)info;
	//const char* type, const char* key,
	if (!strcmp(baseInfo->type, "enc")) {
		table.encTable.Add(baseInfo->key,(EncInfo_t*)info);
	}
	else if(!strcmp(baseInfo->type, "dec")) {
		table.decTable.Add(baseInfo->key, (DecInfo_t*)info);
	}
	return true;
}

bool get_opus_test_info(const char* type, void* info) {
	if (!strcmp(type, "enc")) {
		*(EncInfo_t**)info = 0;
		if(table.encTable.Get(table.encGetNum)->key)
			*(EncInfo_t**)info = &table.encTable.Get(table.encGetNum)->val;
		table.encGetNum++;
	}
	else if (!strcmp(type, "dec")) {
		*(DecInfo_t**)info = 0;
		if(table.decTable.Get(table.decGetNum)->key)
			*(DecInfo_t**)info = &table.decTable.Get(table.decGetNum)->val;
		table.decGetNum++;
	}
	else {
		info = 0;
	}
	return true;
}




#endif