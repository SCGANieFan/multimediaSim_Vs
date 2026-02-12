#include "random.h"
#include <stdio.h>
#define dbgTestPXL(fmt,...) printf(fmt "\n",##__VA_ARGS__)
typedef void (*Func_t)(void* priv, uint16_t idx);
struct TestRandom_t {
    Func_t func;
    uint16_t periodMin;
    uint16_t periodMax;
    uint16_t randomMin;
    uint16_t randomMax;
    uint32_t LoopNum;
    bool running;
};

static void TestRandom(void *priv) {
    TestRandom_t tr = *(TestRandom_t*)priv;
	dbgTestPXL("%p,%u,%u,%u,%u,%u,%d",
		tr.func,
		tr.periodMin,
		tr.periodMax,
		tr.randomMin,
		tr.randomMax,
		tr.LoopNum,
		tr.running);
    uint32_t LoopNum = 0;
    while (LoopNum<tr.LoopNum&& tr.running) {
        uint16_t period = Random(tr.periodMin, tr.periodMax);
        // sleep_for(period);
        uint16_t idx = Random(tr.randomMin, tr.randomMax);
        tr.func(0, idx);
        ++LoopNum;
    }
}

bool TestRandomRegister(Func_t func, uint16_t periodMin, uint16_t periodMax, uint16_t randomMin, uint16_t randomMax, uint32_t LoopNum) {
    TestRandom_t tr;
    tr.func = func;
    tr.periodMin = periodMin;
    tr.periodMax = periodMax;
    tr.randomMin = randomMin;
    tr.randomMax = randomMax;
    tr.LoopNum = LoopNum;
    tr.running = true;
	// thread thr;
	// if(!thr.Start("test", TestRandom, &tr, 4096)) return false;
	// thr.detach();
	return true;
}

