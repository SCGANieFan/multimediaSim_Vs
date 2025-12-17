#pragma once
#include <stdint.h>
#include <stddef.h>

namespace smf {
		class SmfBase {
		public:
			static void* operator new(size_t size);
			static void* operator new(size_t size, void* ptr);
			static void operator delete(void* ptr);
		};

		class SmfNode : public SmfBase {
		public:
			virtual ~SmfNode() {}
		public:
			virtual bool Set(const char* keys, void* vals) = 0;
			virtual bool Get(const char* keys, void* vals) const = 0;
			virtual bool Run(void* para) = 0;
		};

		SmfNode& Tree();
	void* Alloc(unsigned size, unsigned align);
	void* Alloc(unsigned size);
	void* Calloc(unsigned count, unsigned size);
	void* Realloc(void* ptr, unsigned size);
	void Free(void* ptr);
}
