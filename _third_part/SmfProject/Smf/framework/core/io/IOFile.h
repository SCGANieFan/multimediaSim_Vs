#pragma once
#include "IO.h"
#include "smf_api_param.h"
#include <stdio.h>
namespace smf {
	class IOFile :public IO {
	public:
		IOFile();
		virtual ~IOFile();
	protected:
		FILE* _hd;
	public:
		typedef smf_io_param_t OpenParam;
	public:///status ctrl
		virtual bool Open(void*)override;
		virtual bool Close()override;
	public:///status ctrl
		virtual bool Seek(int offset, Position pos)override;
		virtual unsigned GetSize() const override;
		virtual unsigned GetOffset() const override;
		virtual unsigned Read(void* buff, unsigned size) override;
		virtual unsigned Write(void* buff, unsigned size) override;
	protected:
		using IO::dbgErrProcess;
		void dbgErrProcess(const char* file, unsigned line, unsigned error)const;
	};
}

EXTERNC void smf_io_file_register();

