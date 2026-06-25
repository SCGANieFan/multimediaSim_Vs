#include "Measure.h"
#include "SmfPorting.h"
#include "SmfDef.gbl.h"
#include "smf_debug.h"
using namespace smf;
#define meause_get_ticks() get_ticks()
#define meause_lock() os_lock()
#define meause_unlock(v) os_unlock()

Measure::Measure(Value& value)
	: _tick0(meause_get_ticks())
	, _value(value)
{
}
Measure::~Measure() {
	_value.Update(_tick0, meause_get_ticks());
	//dbgTestPXL("[%d]%d,%d(%d,%d)%d",_value.count,tt,_value.used/_value.count,_value.min,_value.max, _value.used);
}

MeasureLock::~MeasureLock() {
	_value.Update(_tick0, meause_get_ticks());
	if(_lock)meause_unlock(_lock);
}
MeasureLock::MeasureLock(Value& value)
	: _lock(0)
	, _tick0(0)
	, _value(value)
{
	if (GlobalFlagsGet(eGlobalFlags::measureLock)) {
		_lock = meause_lock();
	}
	_tick0 = meause_get_ticks();
}

void MeasureValue::Reset() {
	used = 0;
	total = 0;
	count = 0;
	last = 0;
	max = 0;
	min = 0xffffffff;
}
void MeasureValue::Update(uint32_t tick0, uint32_t tick1) {
	auto tt = tick1 - tick0;
	if (used) {
		auto tx = tick1 - last;
		total += tx;
	}
	else {
		total = tt;
	}
	last = tick1;
	count++;
	used += tt;
	if (max < tt)max = tt;
	if (min > tt)min = tt;
}

//uint32_t MeasureValue::kips_avg(uint32_t fus)const { return (fus && count) ? (uint32_t)(used * 1000ull / (fus * count)) : 0ul; }
//uint32_t MeasureValue::kips_min(uint32_t fus)const { return fus ? (uint32_t)(min * 1000ull / fus) : 0; }
//uint32_t MeasureValue::kips_max(uint32_t fus)const { return fus ? (uint32_t)(max * 1000ull / fus) : 0; }
//uint32_t MeasureValue::mips_avg(uint32_t fus)const { return (fus && count) ? (uint32_t)(used / (fus * count)) : 0ul; }
//uint32_t MeasureValue::mips_min(uint32_t fus)const { return fus ? (uint32_t)(min / fus) : 0; }
//uint32_t MeasureValue::mips_max(uint32_t fus)const { return fus ? (uint32_t)(max / fus) : 0; }

uint32_t MeasureValue::ratio_avg(uint32_t base) const { return total ? (uint32_t)((uint64_t)used * base / total) : 0ul; }
uint32_t MeasureValue::ratio_min(uint32_t base) const { return total ? (uint32_t)((uint64_t)min * count * base / total) : 0ul; }
uint32_t MeasureValue::ratio_max(uint32_t base) const { return total ? (uint32_t)((uint64_t)max * count * base / total) : 0ul; }

uint32_t MeasureValue::kips_avg()const { return total ? (uint32_t)((uint64_t)used * get_cpu_freq() / ((uint64_t)total * 1000)) : 0ul;}
uint32_t MeasureValue::kips_min()const { return total ? (uint32_t)((uint64_t)min * count * get_cpu_freq() / ((uint64_t)total * 1000)) : 0ul;}
uint32_t MeasureValue::kips_max()const { return total ? (uint32_t)((uint64_t)max * count * get_cpu_freq() / ((uint64_t)total * 1000)) : 0ul;}

uint32_t MeasureValue::mips_avg()const { return total ? (uint32_t)((uint64_t)used * get_cpu_freq() / ((uint64_t)total * 1000000)) : 0ul; }
uint32_t MeasureValue::mips_min()const { return total ? (uint32_t)((uint64_t)min * count * get_cpu_freq() / ((uint64_t)total * 1000000)) : 0ul; }
uint32_t MeasureValue::mips_max()const { return total ? (uint32_t)((uint64_t)max * count * get_cpu_freq() / ((uint64_t)total * 1000000)) : 0ul; }

void MeasureValue::kips(smf_value_t& val) {
	kips(val.avg, val.min, val.max);
	val.cnt = count;
}
void MeasureValue::kips(uint32_t& avg_, uint32_t& min_, uint32_t& max_) {
	uint64_t freq = get_cpu_freq();
	if (total) {
		auto base = total * 1000;
		auto base_2 = base >> 1;
		avg_ = (freq * used + base_2) / base;
		min_ = (freq * min * count + base_2) / base;
		max_ = (freq * max * count + base_2) / base;
	}
}

void MeasureValue::mips(uint32_t& avg_, uint32_t& min_, uint32_t& max_) {
	uint64_t freq = get_cpu_freq();
	if (total) {
		auto base = total * 1000000;
		auto base_2 = base >> 1;
		avg_ = (freq * used + base_2) / base;
		min_ = (freq * min * count + base_2) / base;
		max_ = (freq * max * count + base_2) / base;
	}
}

MeasureLog::MeasureLog(
	const char* file
	, uint32_t line
	, const char* func
	, const char* name
	, uint32_t index
	, bool lock
) {
	_file = file;
	_line = line;
	_func = func;
	_name = name;
	_index = index;
	if (lock) {
		_lock = meause_lock();
	}
	_ms = get_ms();
}
MeasureLog::~MeasureLog() {
	auto ms = get_ms();
	if (_lock) {
		meause_unlock(_lock);
	}
	__dbgPrintfs("test", dbg_chn_test, _file, _line, _func, DBG_OUTPUT_CHN | DBG_OUTPUT_POS | DBG_OUTPUT_END
		, "%s(%u)%dms,%u", _name, _index, ms - _ms, _ms);
}
