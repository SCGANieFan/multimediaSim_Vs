#include "Object.h"
#include "TaskScheduler.h"

namespace smf {
	class BackGround
		: public Object::List<16, Object, Object>
		, public TaskScheduler
	{
	private:
		using Base = Object;
	public:
		BackGround();
		virtual ~BackGround();
	public:
		bool InvokeDelete(Object* obj, uint32_t delay = 0);
	protected:
		virtual bool set(uint32_t keys, void* vals)override;
	public:
		class Item : public Object {
		protected:
			TaskScheduler::task_t* _task = 0;
			uint32_t _index = 0;
		protected:
			virtual bool set(uint32_t keys, void* vals)override;
		public:
			bool Invoke(uint32_t delay, uint32_t interval = 0);
			bool Interval(uint32_t ms);
		};
	};
}
