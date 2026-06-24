#pragma once
#include "ObjectStatic.h"
#include "IO.h"
#include "unique_ptr.h"
namespace smf {
	class URL :public ObjectStatic {
	public:
		URL(const char* url=0);
		~URL();
	public:
		bool Parse(const char* url);
		void Reset();
		//bool Write(const void* data, int size) const;
		bool Write(const void* data, int size, int offset = 0) const;
		bool Read(VoidBuffer& buff) const;
		bool Read(void* data, int& size) const;
		bool Read(void* data, int& size, int& offset) const;
		const char* url()const { return _buff; }
	private:
		char* _buff = 0;
	public:
		const char* _url = 0;
		const char* _type = 0;
		const char* _path = 0;
		const char* _ext = 0;
		const char* _para = 0;
		const char* _script = 0;
		void* _other = 0;
	public:
		IO* Create(const char*para = 0) const;
	public:
		static IO* Create(const char* url, const char* para);
		static bool Read(const char* url, VoidBuffer& buff);
		static bool Read(const char* url, void* data, int& size);
		static bool Read(const char* url, void* data, int& size, int& offset);
		static bool Write(const char* url, const void* data, int size, int offset = 0);
		static bool Exist(const char* url);
	};
}