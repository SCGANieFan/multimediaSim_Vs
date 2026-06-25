#pragma once
#include "smf_common.h"
#include "SmfMsg.def.h"
namespace smf {
	namespace msg {
		namespace core {
			enum class eCore {
				set = SMF_MSG_CORE,//2
				setResponse,
				config,//4
				configResponse,
				invoke,
				invokeResponse,
			};

			struct set_t {
				uint16_t path;
				uint16_t params;
				char buff[4];
			};

			struct config_t {
				uint16_t script;
				uint16_t params;
				char buff[4];
			};

			struct invoke_t {
				smf_cb_priv0 cb;
				void* priv;
			};
		}
	}
}
