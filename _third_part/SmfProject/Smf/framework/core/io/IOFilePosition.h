#pragma once
#include "IO.h"
#include "unique_ptr.h"
#include "smf_api_param.h"
namespace smf {
    class IOFilePosition : public IO {
    public:
		IOFilePosition();
		// virtual ~IOFilePosition();
	protected:
		smf_keys_value_t _positions[32];
		int _curIdx = 0;
		int _curOffset = 0;
		unique_ptr<IO> _io;
		int _offset = 0;
		mutable int _total = 0;
	public:
		typedef smf_io_param_t OpenParam;
		virtual bool Open(void*)override;
		virtual bool Close()override;
	public:
		virtual bool Seek(int offset, Position pos)override;
		virtual unsigned GetSize() const override;
		virtual unsigned GetOffset() const override;
		virtual unsigned Read(void* buff, unsigned size) override;
		virtual unsigned Write(void* buff, unsigned size) override;
	protected:
		virtual bool set(uint32_t key, void* val) override;
	protected:
		bool initPositonList(smf_keys_value_t*);
		bool initPositonList(const char*);
		int findPositonIdx(int& offset);
    };
}