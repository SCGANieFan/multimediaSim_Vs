#include "IMsvc.h"
#include "SmfDef.gbl.h"
#include "smf_debug.h"

using namespace smf;
using namespace smf::api;
//
IMsvc* IMsvc::Msvc() {
	return smf::Global<IMsvc>(eGlobal::msvc);
}

