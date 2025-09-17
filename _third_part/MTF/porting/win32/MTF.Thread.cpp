#include <windows.h>
//#include <thread>
#include "MTF.Porting.h"

mtf_u32 ThreadIdPorting() {
#if 0
    std::thread::id threadId = std::this_thread::get_id();
    DWORD threadId = GetCurrentThreadId();
    std::cout << "Current Thread ID: " << threadId << std::endl;
    return 0;

#endif
    DWORD threadId = GetCurrentThreadId();
    return threadId;
}




