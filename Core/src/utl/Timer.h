#pragma once
#include <chrono>
#include <iostream>
#include <concepts>
#include <Core/src/log/Log.h>
#include <Core/src/utl/String.h>

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
		void operator()(const char* name, long double duration)
		{
			printf("%.5f ms  %s ", duration, name);
		}

		static DefaultTimerCallback& Get()
		{
			static DefaultTimerCallback singleton;
			return singleton;
		}
	};

	class LogTimerCallback
	{
	public:
		void operator()(const char* name, long double duration)
		{
			trylog.info(ToWide(std::format("{} {:.10f}ms", name, duration)));
		}

		static LogTimerCallback& Get()
		{
			static LogTimerCallback singleton;
			return singleton;
		}
	};

	template <TimerCallbackConcept TimerCallback = DefaultTimerCallback>
	class Timer
	{
	public:
		Timer(const char* name, TimerCallback callback = TimerCallback::Get())
			:
			name(name), callback(callback), isStopped(false)
		{
			start = ch::high_resolution_clock::now();
		}
		~Timer()
		{
			auto end = ch::high_resolution_clock::now();

			auto duration = static_cast<long double>(ch::duration_cast<ch::milliseconds>(end - start).count());

			callback(name, duration);
		}

	private:
		const char* name;
		TimerCallback callback;
		ch::steady_clock::time_point start;
		bool isStopped;
	};
}