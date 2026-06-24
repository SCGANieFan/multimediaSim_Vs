#include "Object.h"
#include "smf_debug.h"

using namespace smf;
//api
EXTERNC void smf_log_background_service(uint32_t interval) {
	if (interval) {
		const char* serial = "slog,slog,0,loop=$0;";
		uint32_t paras[]{ interval };
		returnIfErrC0(!Deserialize(serial, paras, "bg"));
	}
	else {
		Root().Set("bg/slog/en", false);
	}
}

EXTERNC void smf_log_background_save(const char* url, uint32_t duration) {
	const char* serial = "slog,slog,0,duration=$0,url=$1;";
	uint32_t paras[]{ duration,(uint32_t)url };
	returnIfErrC0(!Deserialize(serial, paras, "bg"));
}
