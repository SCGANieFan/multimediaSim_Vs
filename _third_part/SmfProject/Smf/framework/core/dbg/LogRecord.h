#pragma once
#include "Object.h"
#include <stdarg.h>
namespace smf {
	class LogRecord: public Object {
	public:
		LogRecord(int max, int ch);
		virtual ~LogRecord();
	protected:
		int _ch = -1;
		int _max = 256;
		char* _buff = 0;
		char* _begin = 0;
		char* _ptr = 0;
		char* _end = 0;
	public:
		using Object::Set;
		void Set(void* buff, int size);
		void Append(const char* fmt, ...);
		void Append(const char* fmt, va_list& ap);
		void Output();
		void Clear();
	};

	template<int max>
	class TLogRecord :public LogRecord {
	public:
		TLogRecord(int ch)
			:LogRecord(max,ch)
		{
			_buff = _buff0;
		}
		virtual ~TLogRecord() {
			_buff = 0;
		}
	private:
		char _buff0[max];
	};
}
