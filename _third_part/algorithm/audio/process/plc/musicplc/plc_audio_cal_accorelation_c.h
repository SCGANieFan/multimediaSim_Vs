#pragma once

#include "plc_math_c.h"
#include "plc_audio_cal_com.h"
#include "plc_audio_data_c.h"
#include "plc_audio_cal_basic_c.h"
#if 1


template<class Tref, class Ttmp, class Tnorm, int _rShift1 = 0, int _rShift0 = 0>
struct CrossCorr_t{
	static inline Tnorm runAllCh(Tref* ref, Ttmp* temp, i16 channels, i32 accorrelationSample, Tnorm* normRef){
		Tnorm corr = 0;
		Tnorm normRefTmp = 0;
		Tnorm normTempTmp = 0;
		Tref* pRef = ref;
		Ttmp* pTemp = temp;
		for (i32 as = 0; as < accorrelationSample; ++as) {
			for (i16 ch = 0; ch < channels; ++ch) {
				Tnorm tmp = static_cast<Tnorm>(*pRef);
				corr += RShift_c::Run<_rShift0>(tmp * *pTemp);
				normRefTmp += RShift_c::Run<_rShift0>(tmp * tmp);
				++pRef;
				++pTemp;
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normRefTmp = RShift_c::Run<_rShift1>(normRefTmp);
		*normRef = normRefTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normRefTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
	}
	
	static inline Tnorm runCh(Tref* ref, Ttmp* temp, i16 channels, u16 chSelect, i32 accorrelationSample, Tnorm* normRef){
		Tnorm corr = 0;
		Tnorm normRefTmp = 0;
		for (i16 ch = 0; ch < channels; ++ch) {
			if (chSelect & (1 << ch)) {
				Tref* pRef = ref + ch;
				Ttmp* pTemp = temp + ch;
				for (i32 as = 0; as < accorrelationSample; ++as) {
					Tnorm tmp = static_cast<Tnorm>(*pRef);
					corr += RShift_c::Run<_rShift0>( tmp * *pTemp);
					normRefTmp += RShift_c::Run<_rShift0>(tmp * tmp);
					pRef += channels;
					pTemp += channels;
				}
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normRefTmp = RShift_c::Run<_rShift1>(normRefTmp);
		*normRef = normRefTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(*normRef);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
	}
};

template<class Tref, class Ttmp, class Tnorm, int _rShift1>
struct CrossCorr_t<Tref,Ttmp,Tnorm, _rShift1, 0> {
	static inline Tnorm runAllCh(Tref* ref, Ttmp* temp, i16 channels, i32 accorrelationSample, Tnorm* normRef) {
		Tnorm corr = 0;
		Tnorm normRefTmp = 0;
		Tnorm normTempTmp = 0;
		Tref* pRef = ref;
		Ttmp* pTemp = temp;
		for (i32 as = 0; as < accorrelationSample; ++as) {
			for (i16 ch = 0; ch < channels; ++ch) {
				Tnorm tmp = static_cast<Tnorm>(*pRef);
				corr += tmp * *pTemp;
				normRefTmp += tmp * tmp;
				++pRef;
				++pTemp;
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normRefTmp = RShift_c::Run<_rShift1>(normRefTmp);
		*normRef = normRefTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normRefTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
	}

	static inline Tnorm runCh(Tref* ref, Ttmp* temp, i16 channels, u16 chSelect, i32 accorrelationSample, Tnorm* normRef) {
		Tnorm corr = 0;
		Tnorm normRefTmp = 0;
		for (i16 ch = 0; ch < channels; ++ch) {
			if (chSelect & (1 << ch)) {
				Tref* pRef = ref + ch;
				Ttmp* pTemp = temp + ch;
				for (i32 as = 0; as < accorrelationSample; ++as) {
					Tnorm tmp = static_cast<Tnorm>(*pRef);
					corr += tmp * *pTemp;
					normRefTmp += tmp * tmp;
					pRef += channels;
					pTemp += channels;
				}
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normRefTmp = RShift_c::Run<_rShift1>(normRefTmp);
		*normRef = normRefTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(*normRef);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
	}
};

template<class Tref, class Ttmp, class Tnorm, int _rShift1 = 0, int _rShift0 = 0>
struct CrossCorrAccumulate_t {
	static INLINE Tnorm RunAllCh(Tref* ref, Ttmp* temp, i16 channels, i32 accorelationSamples, Tnorm* norm) {
		Tnorm corr = 0;
		Tnorm normTmp = 0;
		i16 i;
		Tref* pRef = ref;
		Ttmp* pTmp = temp;
		for (i = -channels; i < 0 * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			normTmp -= RShift_c::Run<_rShift0>(tmp*tmp);
		}
		for (i = (accorelationSamples - 1) * channels; i < accorelationSamples * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			normTmp += RShift_c::Run<_rShift0>(tmp * tmp);
		}
		for (i = 0; i < accorelationSamples * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			corr += RShift_c::Run<_rShift0>(tmp * pTmp[i]);
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normTmp = RShift_c::Run<_rShift1>(normTmp);
		normTmp += *norm;
		*norm = normTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
	}

	static INLINE Tnorm RunCh(Tref* ref, Ttmp* temp, i16 channels, u16 chSelect, i32 accorelationSamples, Tnorm* norm) {
		Tnorm corr = 0;
		Tnorm normTmp = 0;
		i16 i;
		//last sample
		for (i16 ch = 0; ch < channels; ++ch) {
			if (chSelect & (1 << ch)) {
				Tref* pRef = ref + ch;
				Ttmp* pTmp = temp + ch;
				i = -channels;
				Tnorm tmp = static_cast<Tnorm>(pRef[i]);
				normTmp -= RShift_c::Run<_rShift0>(tmp * tmp);
				tmp = static_cast<Tnorm>(pRef[i + accorelationSamples * channels]);
				normTmp += RShift_c::Run<_rShift0>(tmp * tmp);
				for (i = 0; i < accorelationSamples; ++i) {
					tmp = static_cast<Tnorm>(pRef[i]);
					corr += RShift_c::Run<_rShift0>(tmp * pTmp[i]);
					pRef += channels;
					pTmp += channels;
				}
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normTmp = RShift_c::Run<_rShift1>(normTmp);
		normTmp += *norm;
		*norm = normTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
	}
};

template<class Tref, class Ttmp, class Tnorm, int _rShift1>
struct CrossCorrAccumulate_t<Tref, Ttmp, Tnorm, _rShift1, 0> {
	static INLINE Tnorm RunAllCh(Tref* ref, Ttmp* temp, i16 channels, i32 accorelationSamples, Tnorm* norm) {
#if 0
		Tnorm corr = 0;
		Tnorm normTmp = 0;
		i16 i;
		Tref* pRef = ref;
		Ttmp* pTmp = temp;
		for (i = -channels; i < 0 * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			normTmp -= tmp * tmp;
		}
		for (i = (accorelationSamples - 1) * channels; i < accorelationSamples * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			normTmp += tmp * tmp;
		}
		for (i = 0; i < accorelationSamples * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			corr += tmp * pTmp[i];
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normTmp = RShift_c::Run<_rShift1>(normTmp);
		normTmp += *norm;
		*norm = normTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
#else
		Tnorm corr = 0;
		Tnorm normTmp = 0;
		Tref* pRef = ref;
		Ttmp* pTmp = temp;
		i32 sampleAllCh = accorelationSamples * channels;
		Tref* pRef0 = &pRef[-channels];
		Tref* pRef1 = pRef0 + sampleAllCh;
		for (i16 i = 0; i < channels; ++i) {
			Tnorm tmp0 = static_cast<Tnorm>(pRef0[i]);
			Tnorm tmp1 = static_cast<Tnorm>(pRef1[i]);
			normTmp -= tmp0 * tmp0;
			normTmp += tmp1 * tmp1;
		}

		i32 sampleAllChFrac = sampleAllCh & 3;
		i32 sampleAllChInt = sampleAllCh - sampleAllChFrac;
		pRef0 = &pRef[0];
		Ttmp* pTmp0 = &pTmp[0];
		for (i16 i = 0; i < sampleAllChInt; i+=4) {
			Tnorm tmp0 = static_cast<Tnorm>(pRef0[0]);
			Tnorm tmp1 = static_cast<Tnorm>(pRef0[1]);
			Tnorm tmp2 = static_cast<Tnorm>(pRef0[2]);
			Tnorm tmp3 = static_cast<Tnorm>(pRef0[3]);
			corr += tmp0 * pTmp0[0];
			corr += tmp1 * pTmp0[1];
			corr += tmp2 * pTmp0[2];
			corr += tmp3 * pTmp0[3];
			pRef0 += 4;
			pTmp0 += 4;
		}
		for (i16 i = 0; i < sampleAllChFrac; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef0[0]);
			corr += tmp * pTmp0[0];
			pRef0 += 1;
			pTmp0 += 1;
		}

		corr = RShift_c::Run<_rShift1>(corr);
		normTmp = RShift_c::Run<_rShift1>(normTmp);
		normTmp += *norm;
		*norm = normTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
#endif
	}
	static INLINE Tnorm RunCh(Tref* ref, Ttmp* temp, i16 channels, u16 chSelect, i32 accorelationSamples, Tnorm* norm) {
#if 0
		Tnorm corr = 0;
		Tnorm normTmp = 0;
		i16 i;
		//last sample
		for (i16 ch = 0; ch < channels; ++ch) {
			if (chSelect & (1 << ch)) {
				Tref* pRef = ref + ch;
				Ttmp* pTmp = temp + ch;
				i = -channels;
				Tnorm tmp = static_cast<Tnorm>(pRef[i]);
				normTmp -= tmp * tmp;
				tmp = static_cast<Tnorm>(pRef[i + accorelationSamples * channels]);
				normTmp += tmp * tmp;
				for (i = 0; i < accorelationSamples; ++i) {
					tmp = static_cast<Tnorm>(pRef[i]);
					corr += tmp * pTmp[i];
					pRef += channels;
					pTmp += channels;
				}
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normTmp = RShift_c::Run<_rShift1>(normTmp);
		normTmp += *norm;
		*norm = normTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
#else
		Tnorm corr = 0;
		Tnorm normTmp = 0;
		i16 i;
		if (chSelect == 0x0001) {
			Tref* pRef = ref;
			Ttmp* pTmp = temp;
			Tnorm tmp = static_cast<Tnorm>(pRef[-channels]);
			normTmp -= tmp * tmp;
			tmp = static_cast<Tnorm>(pRef[(accorelationSamples - 1) * channels]);
			normTmp += tmp * tmp;
			i32 accorelationSamplesFrac = accorelationSamples & 3;
			i32 accorelationSamplesInt = accorelationSamples - accorelationSamplesFrac;
			i32 channelsP4 = 4 * channels;
			Tref* pRef0 = ref;
			Tref* pRef1 = pRef0 + channels;
			Tref* pRef2 = pRef1 + channels;
			Tref* pRef3 = pRef2 + channels;
			Ttmp* pTmp0 = temp;
			Ttmp* pTmp1 = pTmp0 + channels;
			Ttmp* pTmp2 = pTmp1 + channels;
			Ttmp* pTmp3 = pTmp2 + channels;
			for (i = 0; i < accorelationSamplesInt; i+=4) {
#if 0
				Tnorm tmp0 = static_cast<Tnorm>(pRef0[0]);
				Tnorm tmp1 = static_cast<Tnorm>(pRef1[0]);
				Tnorm tmp2 = static_cast<Tnorm>(pRef2[0]);
				Tnorm tmp3 = static_cast<Tnorm>(pRef3[0]);
				corr += tmp0 * pTmp0[0];
				corr += tmp1 * pTmp1[0];
				corr += tmp2 * pTmp2[0];
				corr += tmp3 * pTmp3[0];
#else
				Tnorm tmp0 = static_cast<Tnorm>(pRef0[0]);
				Tnorm tmp1 = static_cast<Tnorm>(pRef1[0]);
				Tnorm tmp2 = static_cast<Tnorm>(pRef2[0]);
				Tnorm tmp3 = static_cast<Tnorm>(pRef3[0]);
				corr += tmp0 * pTmp0[0];
				corr += tmp1 * pTmp1[0];
				corr += tmp2 * pTmp2[0];
				corr += tmp3 * pTmp3[0];
#endif
				pRef0 += channelsP4;
				pRef1 += channelsP4;
				pRef2 += channelsP4;
				pRef3 += channelsP4;
				pTmp0 += channelsP4;
				pTmp1 += channelsP4;
				pTmp2 += channelsP4;
				pTmp3 += channelsP4;
			}
			for (i = 0; i < accorelationSamplesFrac; ++i) {
				Tnorm tmp0 = static_cast<Tnorm>(pRef0[0]);
				corr += tmp0 * pTmp0[0];
				pRef0 += channels;
				pTmp0 += channels;
			}
		}
		else {
			for (i16 ch = 0; ch < channels; ++ch) {
				if (chSelect & (1 << ch)) {
					Tref* pRef = ref + ch;
					Ttmp* pTmp = temp + ch;
					i = -channels;
					Tnorm tmp = static_cast<Tnorm>(pRef[i]);
					normTmp -= tmp * tmp;
					tmp = static_cast<Tnorm>(pRef[i + accorelationSamples * channels]);
					normTmp += tmp * tmp;
					i32 accorelationSamplesFrac = accorelationSamples & 3;
					i32 accorelationSamplesInt = accorelationSamples - accorelationSamplesFrac;
					i32 channelsP4 = 4 * channels;
					Tref* pRef0 = ref;
					Tref* pRef1 = pRef0 + channels;
					Tref* pRef2 = pRef1 + channels;
					Tref* pRef3 = pRef2 + channels;
					Ttmp* pTmp0 = temp;
					Ttmp* pTmp1 = pTmp0 + channels;
					Ttmp* pTmp2 = pTmp1 + channels;
					Ttmp* pTmp3 = pTmp2 + channels;
					for (i = 0; i < accorelationSamplesInt; i+=4) {
						Tnorm tmp0 = static_cast<Tnorm>(pRef0[0]);
						Tnorm tmp1 = static_cast<Tnorm>(pRef1[0]);
						Tnorm tmp2 = static_cast<Tnorm>(pRef2[0]);
						Tnorm tmp3 = static_cast<Tnorm>(pRef3[0]);
						corr += tmp0 * pTmp0[0];
						corr += tmp1 * pTmp1[0];
						corr += tmp2 * pTmp2[0];
						corr += tmp3 * pTmp3[0];
						pRef0 += channelsP4;
						pRef1 += channelsP4;
						pRef2 += channelsP4;
						pRef3 += channelsP4;
						pTmp0 += channelsP4;
						pTmp1 += channelsP4;
						pTmp2 += channelsP4;
						pTmp3 += channelsP4;
					}
					for (i = 0; i < accorelationSamplesFrac; ++i) {
						Tnorm tmp0 = static_cast<Tnorm>(pRef0[0]);
						corr += tmp0 * pTmp0[0];
						pRef0 += channels;
						pTmp0 += channels;
					}
				}
			}
		}
		corr = RShift_c::Run<_rShift1>(corr);
		normTmp = RShift_c::Run<_rShift1>(normTmp);
		normTmp += *norm;
		*norm = normTmp;
		Tnorm t1 = math_c<Tnorm>::sqrt(normTmp);
		t1 = math_c<Tnorm>::division(corr, t1 == 0 ? 1 : t1);
		return t1;
#endif
	}
};




#endif