#pragma once
#include <chrono>

namespace ch = std::chrono;

namespace tryn::utl
{
	class Stopwatch
	{
	public:
		void Start()
		{
			start = ch::high_resolution_clock::now();
			running = true;
		}
		std::double_t Stop()
		{
			stop = ch::high_resolution_clock::now();
			duration = stop - start;
			running = false;
			return duration.count() * 10e-10;
		}
		std::double_t Peek()
		{
			const auto now = ch::high_resolution_clock::now();
			return (start - now).count() * 10e-10;
		}
		void Reset()
		{
			duration = ch::nanoseconds::min();
		}
		std::double_t StopAndRestart()
		{
			const auto previous = running ? Stop() : 0.0f;
			Reset();
			Start();
			return previous;
		}
	private:
		ch::steady_clock::time_point start;
		ch::steady_clock::time_point stop;
		bool running = false;
		ch::nanoseconds duration = {};
	};
}