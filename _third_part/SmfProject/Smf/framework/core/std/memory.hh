#pragma once
#include <memory>

namespace smf {
	template<class T>using unique_ptr = std::unique_ptr<T>;
	template<class T>using shared_ptr = std::shared_ptr<T>;
}
