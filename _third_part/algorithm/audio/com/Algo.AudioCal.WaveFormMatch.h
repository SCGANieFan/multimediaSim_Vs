#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioData.h"
#include"Algo.AudioCal.WaveMatchByCrossCor.h"
#if 1

namespace Algo {
namespace Audio {

enum class WaveFormMatchFuncMode_e {
	WAVE_FORM_MATCH_FUNC_MODE_SUM = 0,
	WAVE_FORM_MATCH_FUNC_MODE_ACCORELATION,
};
template<class Tref, class Tcmp, class Tnorm, int _rShitf1 = 0, int _rShitf0 = 0>
class WaveFormMatch_c
{
public:
	class Feature{
	public:
		Feature() {}
		~Feature() {}
		template<class T>
		void Update(T* buf, i16 channels, i32 matchSample) {
			_matchSample = matchSample ? matchSample : 1;
			T* bufOri = buf;
			for (i32 ch = 0; ch < 1; ch++) {
			//for (i32 ch = 0; ch < channels; ch++) {
				T v = *buf;
				vAbsSum[ch] = Abs(v);
				if (v > 0) {
					vPSum[ch] = v;
					vNSum[ch] = 0;
				}
				else {
					vPSum[ch] = 0;
					vNSum[ch] = v;
				}
				vSum[ch] = v;
				vDir[ch] = -v;
				buf += channels;
			}
			for (i32 m = 1; m < matchSample; m++) {
				for (i32 ch = 0; ch < 1; ch++) {
				//for (i32 ch = 0; ch < channels; ch++) {
					T v = *buf;
					vAbsSum[ch] += Abs(v);
					if (v > 0) {
						vPSum[ch] += v;
					}
					else {
						vNSum[ch] += v;
					}
					vSum[ch] += v;
					if (m < matchSample >> 1) {
						vDir[ch] -= v;
					}
					else {
						vDir[ch] += v;
					}
					buf += channels;
				}
			}
		}
		template<class T>
		void Update(T* buf, i16 channels) {
			T* bufOld = &buf[-channels * _matchSample];
			T* bufOld2 = &bufOld[channels * (_matchSample >> 1)];
			for (i32 ch = 0; ch < 1; ch++) {
			//for (i32 ch = 0; ch < channels; ch++) {
				T v = *buf;
				T vOld = *bufOld;
				T vOld2 = *bufOld2;
				vAbsSum[ch] -= Abs(vOld);
				vAbsSum[ch] += Abs(v);
				if (vOld > 0) {
					vPSum[ch] -= vOld;
				}
				else {
					vNSum[ch] -= vOld;
				}
				if (v > 0) {
					vPSum[ch] += v;
				}
				else {
					vNSum[ch] += v;
				}
				vSum[ch] -= vOld;
				vSum[ch] += v;
				vDir[ch] += v + vOld - (vOld2 * 2);
				++buf;
				++bufOld;
				++bufOld2;
			}
		}
		static inline Tnorm Max(Tnorm x0, Tnorm x1) { return x0 > x1 ? x0 : x1; }
		static inline Tnorm Min(Tnorm x0, Tnorm x1) { return x0 < x1 ? x0 : x1; }
		static inline Tnorm Abs(Tnorm x) { return x > 0? x : -x; }
		static Tnorm Evaluate(Feature *ref, Feature *cmp, i16 channels) {
			Tnorm scoreOut = 0;
			for (i32 ch = 0; ch < 1; ch++) {
			//for (i32 ch = 0; ch < channels; ch++) {
				Tnorm score = Abs(ref->vAbsSum[ch] - cmp->vAbsSum[ch]);
				score += Abs(ref->vPSum[ch] - cmp->vPSum[ch]);
				score += Abs(ref->vNSum[ch] - cmp->vNSum[ch]);
				score += Abs(ref->vSum[ch] - cmp->vSum[ch]);
				score += Abs(ref->vDir[ch] - cmp->vDir[ch])*1;
				//score = score / ref->_matchSample;
				scoreOut += score;
			}
			return scoreOut;
		}

	public:
		constexpr static const i16 Channles = 8;
		Tnorm vAbsSum[Channles];
		Tnorm vPSum[Channles];
		Tnorm vNSum[Channles];
		Tnorm vSum[Channles];
		Tnorm vDir[Channles];
		i32 _matchSample;
	};

	static i32 WaveMatchByNFRunCh(Tref* ref, Tcmp* cmp, i16 channels, u16 channelSelect, i32 seekSample, i32 matchSample) {
		return 0;
	}

	static i32 WaveMatchByNFRunAllCh(Tref* ref, Tcmp* cmp, i16 channels, i32 seekSample, i32 matchSample) {
		Feature feaRef;
		Feature feaCmp;
		Tref* pRef = ref;
		Tcmp* pCmp = cmp;
		// s=0
		feaCmp.Update(pCmp, channels, matchSample);
		feaRef.Update(pRef, channels, matchSample);
		Tnorm scoreOpt = Feature::Evaluate(&feaRef, &feaCmp, channels);
		i32 sOpt = 0;
		pRef += channels * matchSample;
		const int stride = 3;
		for (i32 s = stride; s < seekSample; s+= stride) {
			for (i32 str = 0; str < stride; str++) {
				feaRef.Update(pRef, channels);
				pRef += channels;
			}
			Tnorm score = Feature::Evaluate(&feaRef, &feaCmp, channels);
			if (score < scoreOpt) {
				scoreOpt = score;
				sOpt = s;
			}
		}
		return sOpt;
	}
public:
	WaveFormMatch_c() {}
	~WaveFormMatch_c() {}
public:
	INLINE void Init(WaveFormMatchFuncMode_e mode, AudioInfo* info) {
			_info = info;
#if 0
			_waveFormMatchCh = WaveMatchByCrossCor_c<Tref, Tcmp, Tnorm, _rShitf1, _rShitf0>::RunCh;
			_waveFormMatchAllCh = WaveMatchByCrossCor_c<Tref, Tcmp, Tnorm, _rShitf1, _rShitf0>::RunAllCh;
#else
			_waveFormMatchCh = WaveMatchByNFRunCh;
			_waveFormMatchAllCh = WaveMatchByNFRunAllCh;
#endif
	}
	INLINE i32 DoWaveFormMatchCh(Tref* ref, Tcmp* cmp, u16 chSelect, i32 seekSample, i32 matchSample) {
		return _waveFormMatchCh(
			ref,
			cmp,
			_info->_channels,
			chSelect,
			seekSample,
			matchSample);
	}

	INLINE i32 DoWaveFormMatchAllCh(Tref* ref, Tcmp* cmp, i32 seekSample, i32 matchSample) {
		return _waveFormMatchAllCh(
			ref,
			cmp,
			_info->_channels,
			seekSample,
			matchSample);
	}
#if 0
#endif
private:
#if 0
	STATIC i32 WaveFormMatch_Sum(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
	STATIC i32 WaveFormMatch_Accorelation(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
	STATIC i32 WaveFormMatch_Accorelation_AllCh(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
#endif
private:
#if 1
	i32 (*_waveFormMatchCh)(Tref* ref, Tcmp* cmp, i16 channels, u16 channelSelect, i32 seekSample, i32 matchSample);
	i32 (*_waveFormMatchAllCh)(Tref* ref, Tcmp* cmp, i16 channels, i32 seekSample, i32 matchSample);
#if 0
	WaveFormMatch_Sum_CB _waveFormMatch_Sum;
#endif
	AudioInfo* _info;
#endif

};
}
}


#endif