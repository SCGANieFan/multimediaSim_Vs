#pragma once
#include "Object.h"
#include "SmfDef.h"
#include "SimpleTable.h"
#include "mutex.h"
namespace smf {
	class SmfCmd :public Object {
	protected:
		SimpleTable<CbEntry>& _items;
		mutex _mtx;
	protected:
		virtual bool set(uint32_t keys, void* vals)override;
	public:
		SmfCmd(SimpleTable<CbEntry>&);
		int Entry(int argc, char** argv);
		bool Register(const char* name, CbEntry cb);
	public:
		struct cmd_t {
			int argc;
			uint16_t offset[8];
			char buff[0];
		};
		bool Entry(cmd_t*);
		struct argv_t {
			int argc;
			char** argv;
		};
		bool Entry(argv_t*);
	};

	template<int C>
	class SmfCmdC : public SmfCmd {
	protected:
		SimpleTableC<CbEntry,C> _items;
	public:
		SmfCmdC() :SmfCmd(_items) {}
	};
}
