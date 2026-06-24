#pragma once
#include "SmfMsg.def.h"
namespace smf {
	namespace msg {
		enum class eHook {
			hook = SMF_MSG_HOOK,
			hookResponse,
			hookServer,
			hookServerResponse,
		};

		template<int X>
		struct XMsgHook {
			char path[X];
		};
		using MsgHook_t = XMsgHook<0>;
	}
}
