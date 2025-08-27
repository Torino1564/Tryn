
#include "Master.h"
#include <Core/src/utl/Assert.h>

namespace tryn::ccr
{
	Master::Master(int workerCount)
		:
		workerCount(workerCount), doneCount(0)
	{}
	void Master::SignalDone()
	{
		{
			std::lock_guard lock(mtx);
			++doneCount;
		}
		if (doneCount == activeWorkers.value_or(workerCount))
		{
			cv.notify_one();
		}
	}
	void Master::WaitForWorkers(std::optional<int> workersWorking)
	{
		trynass_msg(workersWorking.value_or(workerCount) <= workerCount, L"WaitForWorkers called with a higher number of working threads than the total number of worker threads");
		
		if (workersWorking.has_value())
		{
			activeWorkers = workersWorking;
		}

		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this, workersWorking] {
			return doneCount == workersWorking.value_or(workerCount);
			});
		doneCount = 0;
		activeWorkers = std::nullopt;
	}
	int Master::GetWorkerCount() const
	{
		return workerCount;
	}
}

