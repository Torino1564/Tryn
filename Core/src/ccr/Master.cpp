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
	Worker::Worker(Master* pMaster)
		:
		pMaster_(pMaster)
	{}
	void Worker::StartWorking()
	{
		thread_ = std::jthread(&Worker::WorkerKernel_, this);
	}
	void Worker::StopWorking()
	{
		{
			std::lock_guard lk(mtx_);
			dying = true;
		}
		cv_.notify_one();
	}
	void Worker::SetTask(std::unique_ptr<Task>&& task)
	{
		{
			std::lock_guard lk(mtx_);
			task_ = std::move(task);
			hasJob = true;
		}
		cv_.notify_one();
	}
	void Worker::Kill()
	{
		{
			std::lock_guard lk(mtx_);
			dying = true;
		}
		cv_.notify_one();
	}
	void Worker::WorkerKernel_()
	{
		std::unique_lock lock(mtx_);
		while (true)
		{
			cv_.wait(lock, [this] {
				return hasJob == true || dying;
				});
			if (dying)
			{
				break;
			}
			else [[likely]] {
				trylog.debug(L"Executing Task from Thread");
				task_->Execute();
				hasJob = false;
				pMaster_->SignalDone();
				}
		}
	}
}

