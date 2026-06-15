#pragma once
#include "Object.h"
#include "Semaphore.h"
#include "SmfFifo.h"
#include "mutex.h"
namespace smf {
	class TaskScheduler: public ObjectStatic {
	public:
		TaskScheduler();
		~TaskScheduler();
	public:
		typedef bool(*Func)(void* priv);
	public:
		struct task_t {
			void* priv;
			Func func;
			uint64_t tp;
			uint32_t interval;
			struct task_t* next;
			struct task_t* prev;
		};
		struct item_t {
			void* priv;
			Func func;
		};
	protected:
		task_t _tasks[16];
		task_t* _head = 0;
		SmfFifoS<item_t,8> _fifo;
		semaphore _sema;
		mutex _mtx;
		bool _loop = false;
		bool _pause = false;
		uint16_t _stackSize = 0;
	public:
		bool Enable(bool en, const char* name = 0);
		void Pause(bool pause);
		bool Invoke(Func func, void* priv, uint32_t delay = 0, uint32_t interval = 0, task_t**task = 0);
		bool InvokeDelete(Object* obj, uint32_t delay = 0);
		bool InvokeFree(void* ptr, uint32_t delay = 0);

		mutex& Mutex()const { return (mutex&)_mtx; }
		void StackSize(uint32_t size);
	private:
		bool Insert(task_t*);
		bool Remove(task_t*);
		void Loop();
	};
}
