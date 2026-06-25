#include "smf_common.h"
namespace smf {
	uint32_t get_rtc_ms();
	uint64_t get64_rtc_ms();
	char* rtc_printf(char* ptr, char* end, const char* pattern, void* others);
	char* rtc_printf(char* ptr, char* end);
	char* rtc_printf_short(char* ptr, char* end);
}
