#pragma once
#include <stdint.h>
#include <smf_msg.h>
#include <functional>
namespace smf {
	namespace api {
		class IMsvc {
		public:
			virtual bool Register(const std::function<bool(smf_msg_t*)>& func) = 0;
			virtual bool Receive(smf_msg_t* msg) = 0;
		public:
			virtual const char* GetIpc(uint8_t cpuid) const = 0;
			virtual uint8_t GetIpc(const char* ipcname) const = 0;
			virtual bool IsMaster()const = 0;
		public:
			static IMsvc* Msvc();
		};
	}
}
