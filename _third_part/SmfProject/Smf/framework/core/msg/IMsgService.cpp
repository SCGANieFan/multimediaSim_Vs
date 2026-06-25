#include "IMsgService.h"
#include "IMsgClient.h"
#include "SmfPorting.h"
using namespace smf;
using namespace smf::msg;
//bool IMsgService::InvokeDelete(Object* obj) {
//	returnIfErrC(false, !obj);
//	if (obj->Flags().Check(IS_Destroy))return true;
//	obj->Flags().Set(IS_Destroy, true);
//	return Invoke([obj]() {delete obj; });
//}

