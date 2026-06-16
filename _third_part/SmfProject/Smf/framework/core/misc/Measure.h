#pragma once
#include "smf_common.h"
namespace smf {
	class MeasureValue {
	public:
		uint32_t used = 0;
		uint32_t total = 0;
		uint32_t count = 0;
		uint32_t last = 0;
		uint32_t max = 0;
		uint32_t min = 0xffffffff;
	public:
		void Reset();
		void Update(uint32_t tick0, uint32_t tick1);
	public:
		uint32_t ratio_avg(uint32_t base = 100) const;
		uint32_t ratio_min(uint32_t base = 100) const;
		uint32_t ratio_max(uint32_t base = 100) const;

		uint32_t kips_avg()const;
		uint32_t kips_min()const;
		uint32_t kips_max()const;
		uint32_t mips_avg()const;
		uint32_t mips_min()const;
		uint32_t mips_max()const;

		void kips(smf_value_t& val);
		void kips(uint32_t& avg, uint32_t& min, uint32_t& max);
		void mips(uint32_t& avg, uint32_t& min, uint32_t& max);
	};

	class Measure {
	public:
		using Value = MeasureValue;
		Measure(Value& val);
		~Measure();
	protected:
		uint32_t _tick0;
		Value& _value;
	};

	class MeasureLock {
	public:
		using Value = MeasureValue;
		MeasureLock(Value& val);
		~MeasureLock();
	protected:
		int _lock;
		uint32_t _tick0;
		Value& _value;
	};

	class MeasureLog {
	public:
		MeasureLog(const char* file, uint32_t line, const char* func, const char* name = 0, uint32_t index = 0, bool lock = false);
		~MeasureLog();
	protected:
		int _lock = 0;
		uint32_t _ms = 0;
		const char* _file = 0;
		uint32_t _line = 0;
		const char* _func = 0;
		uint32_t _index = 0;
		const char* _name = 0;
	};
#define XMeasureLog(...) MeasureLog mlog(__FILE__,__LINE__,__func__,##__VA_ARGS__)
}
