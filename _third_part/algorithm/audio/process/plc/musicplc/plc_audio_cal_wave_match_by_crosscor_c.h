#pragma once

#include "plc_math_c.h"
#include "plc_audio_cal_com.h"
#include "plc_audio_cal_accorelation_c.h"
#include "plc_audio_data_c.h"

#if 1

	
template<class Tref, class Tcmp, class Tnorm, int _rShitf1 = 0, int _rShitf0 = 0>
class WaveMatchByCrossCor_c
{
public:
	WaveMatchByCrossCor_c() {}
	~WaveMatchByCrossCor_c() {}

public:
	static i32 RunAllCh(Tref* ref, Tcmp* cmp, i16 channels, i32 seekSample, i32 matchSample) {
		i32 lagOpt = 0;
		Tnorm norm = 0;
		Tnorm bestCorr = 0;
		i32 accorrelationSample = matchSample;
		Tref* pRef = (Tref*)ref;
		bestCorr = CrossCorr_t<Tref,Tcmp,Tnorm,_rShitf1,_rShitf0>::runAllCh(ref, cmp, channels, accorrelationSample, &norm);
		for (i32 s = 1; s < seekSample; s++) {
			pRef += channels;
			Tnorm corr;
			corr = CrossCorrAccumulate_t<Tref,Tcmp,Tnorm,_rShitf1,_rShitf0>::RunAllCh(pRef, cmp, channels, accorrelationSample, &norm);
			if (bestCorr < corr) {
				bestCorr = corr;
				lagOpt = s;
			}
		}
		return lagOpt;
	}

	static i32 RunCh(Tref* ref, Tcmp* cmp, i16 channels, u16 channel_select, i32 seekSample, i32 matchSample) {
		i32 lagOpt = 0;
		Tnorm norm = 0;
		Tnorm bestCorr = 0;
		i32 accorrelationSample = matchSample;
		Tref* pRef = (Tref*)ref;
		bestCorr = CrossCorr_t<Tref, Tcmp, Tnorm, _rShitf1,_rShitf0>::runCh(ref, cmp, channels, channel_select, accorrelationSample, &norm);
		for (i32 s = 1; s < seekSample; s++) {
			pRef += channels;
			Tnorm corr;
			corr = CrossCorrAccumulate_t<Tref, Tcmp, Tnorm, _rShitf1,_rShitf0>::RunCh(pRef, cmp, channels, channel_select, accorrelationSample, &norm);
			if (bestCorr < corr) {
				bestCorr = corr;
				lagOpt = s;
			}
		}
		return lagOpt;
	}
private:
};



#endif