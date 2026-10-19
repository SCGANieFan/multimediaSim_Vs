#include "PcmResampleAdv.h"
#include "smf_debug.h"

using namespace smf;
using namespace smf::audio;

EXTERNC void smf_resample_cb_register(
	int (*get_size)(int bits)
	, bool (*open)(void* hd, int chs, int targetChs, int rate, int targetRate, int bits, int targetBits)
	, bool (*run)(void* hd, const void* inbuf, int* insize, void* outbuf, int* outsize)
	, void (*close)(void* hd)
)
{
	auto& res = AudioResamples();
	res.get_size = get_size;
	res.open = open;
	res.run = run;
	res.close = close;
}
