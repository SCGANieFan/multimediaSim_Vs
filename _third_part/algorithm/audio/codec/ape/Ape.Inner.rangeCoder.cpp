#if 1
#include"Ape.Inner.rangeCoder.h"

/**
 * Probability ranges for symbols in Monkey Audio version 3.97
 */
static const u16 counts_diff_3970[21] = {
    14824, 13400, 11124, 8507, 6139, 4177, 2755, 1756,
    1104, 677, 415, 248, 150, 89, 54, 31,
    19, 11, 7, 4, 2,
};

/**
 * Fixed probabilities for symbols in Monkey Audio version 3.97
 */
static const u16 counts_3970[22] = {
        0, 14824, 28224, 39348, 47855, 53994, 58171, 60926,
    62682, 63786, 64463, 64878, 65126, 65276, 65365, 65419,
    65450, 65469, 65480, 65487, 65491, 65493,
};

/**
 * Fixed probabilities for symbols in Monkey Audio version 3.98
 */
static const u16 counts_3980[22] = {
        0, 19578, 36160, 48417, 56323, 60899, 63265, 64435,
    64971, 65232, 65351, 65416, 65447, 65466, 65476, 65482,
    65485, 65488, 65490, 65491, 65492, 65493,
};

/**
 * Probability ranges for symbols in Monkey Audio version 3.98
 */
static const u16 counts_diff_3980[21] = {
    19578, 16582, 12257, 7906, 4576, 2366, 1170, 536,
    261, 119, 65, 31, 19, 10, 6, 3,
    3, 2, 1, 1, 1,
};

static const u32 K_SUM_MIN_BOUNDARY[32] = {
    0,32,64,128,256,512,1024,2048,
    4096,8192,16384,32768,65536,131072,262144,524288,
    1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,
    268435456,536870912,1073741824,2147483648,0,0,0,0
};


uint32_t ApeRangecoder::range_dec_normalize(BufferRead* bufferRead)
{
	//check
	while (_range <= ((1u << (32 - 1)) >> 8))
	{
		_buffer <<= 8;
		_buffer += bufferRead->Read(1);
		_low = (_low << 8) | ((_buffer >> 1) & 0xFF);
		_range <<= 8;
		if (_range == 0)
			return 0;
	}
	return 0;
}
// i64 frames = 0;
bool ApeRangecoder::range_get_symbol(const uint16_t counts[], const uint16_t counts_diff[], BufferRead* bufferRead, int32_t* symbol)
{
	int32_t cf;
	int32_t out;
	range_dec_normalize(bufferRead);
	_range = _range >> 16;
	cf = _low / _range;
	if (cf > 65492) {
		out = cf - 65535 + 63;
		_low -= _range * cf;
		if (cf > 65535)
			return false;
		*symbol = out;
		return true;
	}
	/* figure out the symbol inefficiently; a binary search would be much better */
	for (out = 0; counts[out + 1] <= cf; out++);
	_low -= _range * counts[out];
	_range = _range * counts_diff[out];
	*symbol = out;
	return true;
}

int32_t ApeRangecoder::range_decode_bits(int n, BufferRead* bufferRead)
{
	int32_t sym;
	range_dec_normalize(bufferRead);
	_range = _range >> n;
	sym = _low / _range;
	_low -= _range * sym;
	return sym;
}

bool ApeRangecoder::EntropyDecode(ApeRice* rice, BufferRead* bufferRead, int32_t* out, uint16_t version)
{
	//init
	int32_t x;
	int32_t overflow;
	//get x
	if (version < 3990)
	{
		int32_t tmpk;
		if (!range_get_symbol(counts_3970, counts_diff_3970, bufferRead, &overflow))
			return false;

		if (overflow == (MODEL_ELEMENTS - 1)) {
			tmpk = range_decode_bits(5, bufferRead);
			overflow = 0;
		}
		else
			tmpk = (rice->k < 1) ? 0 : rice->k - 1;

		if (tmpk <= 16)
			x = range_decode_bits(tmpk, bufferRead);
		else {
			x = range_decode_bits(16, bufferRead);
			x |= (range_decode_bits(tmpk - 16, bufferRead) << 16);
		}
		x += overflow << tmpk;
	}
	else
	{
		int32_t base, pivot;

		pivot = rice->ksum >> 5;
		if (pivot == 0)
			pivot = 1;
		if (!range_get_symbol(counts_3980, counts_diff_3980, bufferRead, &overflow))
			return false;
		if (overflow == (MODEL_ELEMENTS - 1))
		{
			overflow = range_decode_bits(16, bufferRead) << 16;
			overflow |= range_decode_bits(16, bufferRead);
			//// detect overflow signaling here, adjust the pivot value and try again
			//if (overflow == 1)
			//{
			//	pivot = 32768;
			//	return DecodeOverflow(nPivotValue);
			//}
		}

		if (pivot < 0x10000)
		{
			range_dec_normalize(bufferRead);
			_range = _range / pivot;
			base = _low / _range;
			_low -= _range * base;
		}
		else
		{
			int32_t base_hi = pivot, base_lo;
			int32_t bbits = 0;

			while (base_hi & ~0xFFFF) {
				base_hi >>= 1;
				bbits++;
			}
			range_dec_normalize(bufferRead);
			_range = _range / (base_hi + 1);

			base_hi = _low / _range;
			_low -= _range * base_hi;

			range_dec_normalize(bufferRead);
			_range = _range / (1 << bbits);

			base_lo = _low / _range;
			_low -= _range * base_lo;
			base = (base_hi << bbits) + base_lo;
		}

		x = base + overflow * pivot;
	}
	rice->ksum += ((x + 1) / 2) - ((rice->ksum + 16) >> 5);

	if (rice->ksum < K_SUM_MIN_BOUNDARY[rice->k])
		rice->k--;
	else if (rice->ksum >= K_SUM_MIN_BOUNDARY[rice->k + 1])
		rice->k++;

	//get out
	//Convert to signed
	if (x & 1)
		*out = (x >> 1) + 1;
	else
		*out = -(x >> 1);

	return true;
}








#endif
