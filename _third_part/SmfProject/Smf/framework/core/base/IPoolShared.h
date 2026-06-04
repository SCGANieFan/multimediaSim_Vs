#pragma once
namespace smf{
	class IPoolShared {
	public:
		void Init(int idx);
	public:
		void* Alloc(const char* name, unsigned size, int align) { return alloc(name, 0, size, align); }
		void* Alloc(const char* name, unsigned& size) { return alloc(name, 0, size, 0); }
		void* Alloc(const char* name) { unsigned size = 0; return alloc(name, 0, size, 0); }
		void* Alloc(void* buff, unsigned& size) { return alloc(0, buff, size, 0); }
		void* Alloc(void* buff) { unsigned size = 0; return alloc(0, buff, size, 0); }
		void* Alloc(unsigned size) { return alloc(0, 0, size, 0); }
		bool Free(const char* name) { return free(name, 0); }
		bool Free(void* buff) { return free(0, buff); }
	public:
		void* Alloc(const char* name, void* buff, unsigned& size, int align) { return alloc(name, buff, size, align); }
		bool Free(const char* name, void* buff) { return free(name, buff); }
		virtual bool IsLocal()const = 0;
	protected:
		virtual void* alloc(const char* name, void* buff, unsigned& size, unsigned align) = 0;
		virtual bool free(const char* name, void* buff) = 0;
	};
}
