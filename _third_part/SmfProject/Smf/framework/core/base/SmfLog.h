#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <mutex.h>
namespace smf {
	class SmfLog{
	public:
		SmfLog();
	public:
		typedef void(*Func1)(const char* str);
		typedef void(*Func2)(const char* str, int size);
		typedef void(*Func3)(const char* str, int size, int ch);
		typedef void(*Func4)(const char* str, int size, int ch, void* priv);
		using Func = Func4;
		void Set(void* buff, int size);
		void Set(Func1 func);
		void Set(Func2 func);
		void Set(Func3 func);
		void Set(Func4 func, void* priv= 0);
		bool Set(uint32_t keys, void* vals);
	protected:
		char* _buff = 0;
		int _size = 0;
		void* _priv = 0;
		Func _output = 0;
		//
		uint32_t _channelDisable = 0;
		mutex _mtx;
		uint32_t _total = 0;
		uint32_t _loss = 0;
		uint32_t _cache = 0;
	public:
		typedef struct {
			const char* name;
			const char* file;
			const char* func;
			const char* fmt;
			uint16_t line;
			int8_t ch;
			bool show_end:1;
			bool show_chn:1;
			bool show_thd:1;
			bool show_pos:1;
			bool show_key:1;
			bool show_other:3;
			va_list* ap;
		}param_t;
	public:
		void ChannelSet(int ch, bool enable);
		void ChannelMap(uint32_t mask, bool enable);
		void ChannelMap(uint32_t mask);	
		uint32_t ChannelMap()const;
	protected:
		void Print(const void* buff, int size, int ch);
	public:
		void Output(const void* buff, int size, int ch);
		void Output(param_t& para, const void* buff, int size);
		void Printf(param_t& para);
		void Dump(param_t& para, const void* data, int size, int line=0);
		//void Dump(param_t& para, void* data, int size, void* buff, int max, int line=0);
		void Dump(int ch, const void* data, int size, int line = 0);
		char* Dump(char* dst, char* end, const void* data, int size, int line);
	public:
		void Printf(const char* pattern, ...);
		void Printf(int ch, const char* name, const char* pattern, ...);
		void Printf(int ch, const char* name, int line, const char* file, const char* func, const char* pattern, ...);
		void Printf(va_list& ap, int ch, const char* name, int line, const char* file, const char* func, const char* pattern);
	protected:
		void printFlags(char*& ptr, char* end, param_t& para);
		//void printChannel(char*& ptr, char* end, param_t& para);
		//void printThread(char*& ptr, char* end, param_t& para);
		void printPosition(char*& ptr, char* end, param_t& para);
		//void printKeys(char*& ptr, char* end, param_t& para);
		//void printType(char*& ptr, char* end, param_t& para);
		bool checkChannel(int n);
	protected:
		int getBuff(char*& buff);
	public:
		char* Print(char* ptr, char* end);
	};

	template<int C>
	class TSmfLog :public SmfLog {
	private:
		char _tbuff[C];
	public:
		TSmfLog() { Set(_tbuff, C); }
	};
}

