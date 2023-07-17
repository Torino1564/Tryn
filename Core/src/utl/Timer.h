#pragma once
#include <chrono>
#include <iostream>
#include <concepts>

namespace ch = std::chrono;

namespace tryn::utl
{
	template <typename Callable>
	concept TimerCallbackConcept = requires(Callable c, const char* name, float duration)
	{
		{ c(name, duration) } -> std::same_as<void>;
	};


	class DefaultTimerCallback
	{
	public:
		void operator()(const char* name, float duration)
		{
			printf( "%.3f  %s", duration, name);
		}

		static DefaultTimerCallback& Get()
		{
			static DefaultTimerCallback singleton;
			return singleton;
		}
	};

	template <TimerCallbackConcept TimerCallback = DefaultTimerCallback>
	class Timer
	{
	public:
		Timer( const char* name , TimerCallback callback = DefaultTimerCallback::Get())
			:
			name(name) , callback(callback) , isStopped(false)
		{
			start = ch::high_resolution_clock::now();
		}
		~Timer()
		{
			auto end = ch::high_resolution_clock::now();

			float duration = static_cast<float>(ch::duration_cast<ch::milliseconds>(end - start).count());

			callback( name , duration );
		}

	private:
		const char* name;
		TimerCallback callback;
		ch::steady_clock::time_point start;
		bool isStopped;
	};
}