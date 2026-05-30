#pragma once
#include <chrono>
#include <iostream>
#include <concepts>

namespace ch = std::chrono;

namespace tryn::utl
{
	template <typename Callable>
	concept TimerCallbackConcept = requires(Callable c, const char* name, long double duration)
	{
		{ c(name, duration) } -> std::same_as<void>;
	};

	class DefaultTimerCallback
	{
	public:
		void operator()(const char* name, long double duration)
		{
			printf("%.5f  %s", duration, name);
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
		Timer(const char* name, TimerCallback callback = DefaultTimerCallback::Get())
			:
			name(name), callback(callback), isStopped(false)
		{
			start = ch::steady_clock::now();
		}
		~Timer()
		{
			auto end = ch::steady_clock::now();

			long double duration = static_cast<long double>((end - start).count() / 1e6 );

			callback(name, duration);
		}

	private:
		const char* name;
		TimerCallback callback;
		ch::steady_clock::time_point start;
		bool isStopped;
	};
}