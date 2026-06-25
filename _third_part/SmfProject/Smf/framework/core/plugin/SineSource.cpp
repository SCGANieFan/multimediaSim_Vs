#include "SineSource.h"
#include "SmfHash.h"
#include "Source.h"
#include "smf_debug.h"
#include <math.h>

using namespace smf;

EXTERNC void smf_sine_source_register() {
	SineSource::Register<SineSource>("src-sine");
}
SineSource::SineSource() {
	InitMedia(this);
	_rate = 16000;
	_channels = 1;
	_sampleBits = 16;
}
SineSource::~SineSource() {
	Status(EStatus::null);
}
bool SineSource::open(void*param) {
	_frame_samples = (_rate * _frame_duration_ms + 500) / 1000;
	_oport->_max = _frame_samples * sizeof(int16_t);
	_table_size = static_cast<size_t>(_rate / _frequency);
	_wave_table = static_cast<int16_t*>(Alloc(_table_size * sizeof(int16_t)));
	returnIfErrC(false, !_wave_table);
	double amplitude_linear = pow(10.0, _amplitude_db / 20.0);
	amplitude_linear = (amplitude_linear > (double)1.0) ? (double)1.0 : amplitude_linear;

	for (size_t i = 0; i < _table_size; ++i) {
		double phase = 2.0 * 3.14159265358979323846 * i / _table_size;
		_wave_table[i] = static_cast<int16_t>(sin(phase) * amplitude_linear * (double)32767.0);
	}

	return Source::open(param);
}
bool SineSource::close() {
	if (_wave_table) {
		Free(_wave_table);
	}
	return true;
}
bool SineSource::generateFrame(Frame*& frm) {
	auto ms1 = get_local_ms();
	if (!_ms0)
		_ms0 = ms1;
	int64_t ms = ms1 - _ms0;
	returnIfErrC(false, !_rate);
	int64_t msTarget = _fcount * 1000ull * _frame_samples / _rate;
	int32_t dms = msTarget - ms;
	_fcount++;

	if (dms > 0) {
		sleep_for(dms);
	}

	uint16_t* odata = (uint16_t*)frm->buff;
	for (size_t i = 0; i < _frame_samples; ++i) {
		odata[i] = _wave_table[_phase];
		_phase = (_phase + 1) % _table_size;
	}
	frm->size = _frame_samples * sizeof(int16_t);

	return true;
}
bool SineSource::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("mute"):
		_mute = (bool)val;
		dbgTestPDL(_mute);
		return true;
	}
	return Source::set(key, val);
}
