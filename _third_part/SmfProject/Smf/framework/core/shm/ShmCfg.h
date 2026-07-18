#pragma once
#include "smf_common.h"
namespace smf {
	struct shmcfg_t {
		uint32_t _timeskip;
		uint32_t _timeout;
		//
		uint64_t _keys;
		uint32_t _buff_max;
		uint32_t _fifo_max;
		uint32_t _sleep_ms;
		//
		uint8_t _sharedIndex;
		bool _isCacheable;
		bool _noCopy;
		bool _isReader;
		//
		bool _isRing;
		bool _noAlloc;
		bool _partread;
		//
		uint8_t revss[5]; //align to 64
	};
}