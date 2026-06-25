#include "TaskScheduler.h"
#include "thread.h"
#include "smf_debug.h"
#include "SmfPorting.h"

using namespace smf;

TaskScheduler::TaskScheduler() {
	_stackSize = 1024 * 6;
	Enable(true);
	memset(_tasks, 0, sizeof(_tasks));
}

TaskScheduler::~TaskScheduler() {
	_loop = false;
	_sema.post();
	sleep_for(1000);
}
void TaskScheduler::StackSize(uint32_t size) {
	if (!size) {
		_stackSize = size;
	}
}
bool TaskScheduler::Enable(bool en, const char* name) {
	if (en) {
		returnIf(true, _loop);
		_loop = true;
		thread thr;
		if (!name)name = "ts";
		thr.Start(name, [](void* priv) {((TaskScheduler*)priv)->Loop(); }, this, _stackSize);
		thr.detach();
	}
	else {
		_loop = false;
		_sema.post();
	}
	return true;
}

void TaskScheduler::Pause(bool pause) {
	if (pause) {
		_pause = true;
	}
	else {
		_pause = false;
		_sema.post();
	}
}

bool TaskScheduler::Invoke(Func func, void* priv, uint32_t delay, uint32_t interval, task_t** ptask){
	unique_lock<mutex> lck(_mtx);
	if (!delay && !interval) {
		returnIfErrC(false,!_fifo.Write({ priv,func }));
		_sema.post();
		return true;
	}
	if (!interval)interval = delay;
	auto tp = get64_ms();
	for (auto& task : _tasks) {
		if (!task.tp) {
			task.tp = tp + delay;
			task.interval = interval;
			task.func = func;
			task.priv = priv;
			task.next = 0;
			task.prev = 0;
			if (ptask)*ptask = &task;
			return Insert(&task);
		}
	}
	dbgErrPL();
	return false;
}

bool TaskScheduler::InvokeDelete(Object* obj, uint32_t delay) {
	//dbgTestPXL("%s,%u", obj->Name(), delay);
	return Invoke([](void* priv) {
		//dbgTestPXL("%s", ((Object*)priv)->Name()); 
		delete (Object*)priv; return true; 
		}, (void*)obj, delay);
	//return Invoke([obj]() {delete obj; return true; }, delay);
}
bool TaskScheduler::InvokeFree(void* ptr, uint32_t delay) {
	return Invoke([](void* priv) {Free(priv); return true; }, ptr, delay);
	//return Invoke([ptr]() {Free(ptr); return true; }, delay);
}

bool TaskScheduler::Insert(task_t* node) {
	node->prev = 0;
	node->next = 0;

	auto curr = _head;
	if (!curr) {//no head
		_head = node;
		_sema.post();//head changed
	}
	else if (node->tp < curr->tp) {//insert before head
		node->next = curr;
		curr->prev = node;
		_head = node;
		_sema.post();//head changed
	}
	else {
		//find insert node
		while (curr->next && curr->next->tp < node->tp) {
			curr = curr->next;
		}
		//insert after curr
		node->prev = curr;
		node->next = curr->next;
		if (curr->next) {
			curr->next->prev = node;
		}
		curr->next = node;
	}
	return true;
}

bool TaskScheduler::Remove(task_t* node) {
	auto next = node->next;
	auto prev = node->prev;
	if (prev)prev->next = next;
	if (next)next->prev = prev;
	node->next = 0;
	node->prev = 0;
	if (_head == node)_head = next;
	return true;
}

void TaskScheduler::Loop() {
	int delay = 0;
	while (_loop) {
		if (_pause) {
			delay = 0x7fffffff;
		}
		if (delay>0) {
			_sema.wait_for(delay);
			if (!_loop)break;
		}
		while (!_fifo.Empty()) {
			item_t item{ 0,0 };
			{
				unique_lock<mutex> lck(_mtx);
				_fifo.Read(item);
			}
			if (item.func) {
				item.func(item.priv);
			}
		}
		task_t* node = _head;
		if (node) {
			auto tp = get64_ms();
			delay = node->tp - tp;
			if (delay > 0) {
				continue;
			}
			if (node->func && !node->func(node->priv)) {//loop
				unique_lock<mutex> lck(_mtx);
				Remove(node);
				//insert again
				node->tp = tp + node->interval;
				Insert(node);
			}
			else {//clear
				unique_lock<mutex> lck(_mtx);
				Remove(node);
				//clear node
				node->tp = 0;
				node->interval = 0;
				node->func = 0;
				node->priv = 0;
			}
		}
		else {
			delay = 0x7fffffff;
		}
	}
}
