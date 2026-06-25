#pragma once
namespace smf {
	template<class TFunc>
	class ExitCall {
	protected:
		TFunc _func;
		bool _enable = true;
	public:
		ExitCall(const TFunc& func) :_func(func) {}
		~ExitCall() {
			if (_enable)_func();
		}
		void Reset() {
			_enable = false;
		}
		void Release() {
			_enable = false;
		}
	};
	
	class ExitCallx1 {
	protected:
		typedef void(*TFunc)(void*);
		TFunc _func;
		void* _priv;
	public:
		template<class T>
		ExitCallx1(const T& func, void* priv) :_func((TFunc)func), _priv(priv) {}
		~ExitCallx1() {
			if (_func)_func(_priv);
		}
		void Reset() {
			_func = 0;
		}
		void Release() {
			_func = 0;
		}
	};
}