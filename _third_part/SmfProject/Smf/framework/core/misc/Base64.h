#pragma once

namespace smf {
	unsigned char* base64Decode(char const* in, unsigned& resultSize, bool trimTrailingZeros);
	char* base64Encode(char const* origSigned, unsigned origLength);
}
