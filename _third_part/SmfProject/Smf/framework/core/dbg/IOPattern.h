#pragma once
#include "IO.h"
#include "string.hh"

namespace smf {
	class IOPattern: public ObjectStatic	{
	public:
		IOPattern();
		~IOPattern();
	public:
		string _pattern;
		uint32_t _duration = 0;
		IO* _ios[8];
	protected:
		IO* create();
	public:
		bool Clear();
		bool Close(uint32_t idx);
		bool Write(uint32_t idx, void* data, uint32_t size);
	};

}
