#pragma once
#include "MediaAudio.h"
#include "Source.h"
#include "mutex.h"

namespace smf {
class SineSource
    : public Source,
      public MediaPcm {
public:
	SineSource();
	virtual ~SineSource();

protected:
	virtual bool open(void*) override;
	virtual bool close() override;
	virtual bool generateFrame(Frame*&) override;

protected:
	virtual bool set(uint32_t key, void* val) override;

protected:
	bool _mute = false;

private:
	int _frequency = 1000;
	int _amplitude_db = -30;
	// int _sample_rate = 16000;
	int _frame_duration_ms = 20;
	int _frame_samples = 0;
	int _table_size = 0;
	int _phase = 0;
	int16_t* _wave_table = 0;
	uint32_t _fcount = 0;
	int64_t _ms0 = 0;
};
} // namespace smf
