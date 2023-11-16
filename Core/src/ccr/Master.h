#pragma once
#include <thread>
#include <future>
#include <span>
#include <functional>
#include <tuple>
#include <Core/src/log/Log.h>
#include <optional>

namespace tryn::ccr
{
	class Master
	{
	public:
		Master(int workerCount);
		void SignalDone();
		void WaitForWorkers(std::optional<int> workersWorking = std::nullopt);
		int GetWorkerCount() const;
	private:
		std::condition_variable cv;
		std::mutex mtx;
		int workerCount;
		std::optional<int> activeWorkers = std::nullopt;
		//Shared memory
		int doneCount = 0;
	};
}