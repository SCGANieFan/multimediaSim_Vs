#include "IO.h"
#include "Object.h"

namespace smf {
	class LogFile {
	public:
		LogFile();
		~LogFile();
	protected:
		IO* _ios[16];
		IO* _io;
		const char* _path;
	public:
		int Output(const char* str, int size, int ch);
	protected:
		int output(IO*&io,const char* str, int size, int ch);
	};
}