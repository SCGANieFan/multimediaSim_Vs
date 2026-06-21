#include "IPool.h"
#include "IPoolShared.h"
#include "IPoolList.h"
using namespace smf;
void IPoolShared::Init(int idx) {
	IPool::Pools()._shared[idx] = this;
}
