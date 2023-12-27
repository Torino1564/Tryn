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
		}
		std::double_t Stop()
		{
			stop = ch::high_resolution_clock::now();
			duration = stop - start;
			return duration.count() * 10e-9;
		}
		std::double_t Peek()
		{
			const auto now = ch::high_resolution_clock::now();
			return (start - now).count() * 10e-9;
		}
		void Reset()
		{
			duration = ch::nanoseconds::min();
		}
		std::double_t StopAndRestart()
		{
			const auto previous = Stop();
			Reset();
			Start();
			return previous;
		}
	private:
		ch::steady_clock::time_point start;
		ch::steady_clock::time_point stop;
		ch::nanoseconds duration = ch::nanoseconds::min();
	};
}