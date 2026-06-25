#pragma once
#include "ObjectStatic.h"
namespace smf {
	class Remote: public ObjectStatic {
	public:
		//Remote();
		~Remote();
	public:
		bool set(const char* script);
		bool set(uint32_t keys, void* vals);
		bool create();
		bool destroy();
		bool valid() const { return _cpuid; }
	protected:
		uint8_t _cpuid = 0;
		const char* _create = 0;
		const char* _destroy = 0;
	};
}
