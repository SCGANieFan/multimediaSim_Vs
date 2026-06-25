#pragma once
#include <stdint.h>
#include "IMsgClient.h"
#include "SmfFifo.h"
#include "Semaphore.h"

namespace smf {
	namespace msg {
		class IMsgClientAsync :public IMsgClient {
		public:
			IMsgClientAsync();
			virtual ~IMsgClientAsync();
		public:
			virtual bool init(IMsgService* svc,bool en) override;
			virtual Result Process(smf_msg_t* msg) override final;
			virtual bool set(uint32_t key,void*val) override;
		protected:
			bool receive(smf_msg_t* msg);
			virtual bool process(smf_msg_t* msg) = 0;
			virtual Result check(smf_msg_t* msg) = 0;
		protected:
			//IMsgService* _svc = 0;
			uint16_t _stackSize = 1024 * 2;
			bool _loop = true;
			SmfFifoS<smf_msg_t*,16> _fifos;
            uint8_t _fifoBuffs[16][128];
			semaphore _sem;
		protected:
			void Loop(void);
		};
	}
}
