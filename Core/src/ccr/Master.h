#pragma once
#include <thread>
#include <future>
#include <span>
#include <functional>
#include <tuple>
#include <Core/src/log/Log.h>

namespace tryn::ccr
{
	class Master
	{
	public:
		Master(int workerCount)
			:
			workerCount(workerCount), doneCount(0)
		{}
		void SignalDone()
		{
			{
				std::lock_guard lock(mtx);
				++doneCount;
			}
			if (doneCount == workerCount)
			{
				cv.notify_one();
			}
		}
		void WaitForWorkers()
		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [this] {return doneCount == workerCount; });
			doneCount = 0;
		}
		int GetWorkerCount() const
		{
			return workerCount;
		}
	private:
		std::condition_variable cv;
		std::mutex mtx;
		int workerCount;
		//Shared memory
		int doneCount = 0;
	};

	class Task
	{
	public:
		virtual void Execute() = 0;
	};

	class Worker
	{
	public:
		Worker() : pMaster_(nullptr) {}
		virtual ~Worker() = default;
		Worker(Master* pMaster)
			:
			pMaster_(pMaster)
		{}
		void StartWorking()
		{
			thread_ = std::jthread(&Worker::WorkerKernel_, this);
		}
		void StopWorking()
		{
			{
				std::lock_guard lk(mtx_);
				dying = true;
			}
			cv_.notify_one();
		}
		void SetTask(std::unique_ptr<Task>&& task)
		{
			{
				std::lock_guard lk(mtx_);
				task_ = std::move(task);
				hasJob = true;
			}
			cv_.notify_one();
		}
		void Kill()
		{
			{
				std::lock_guard lk(mtx_);
				dying = true;
			}
			cv_.notify_one();
		}
	protected:
		void WorkerKernel_()
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
				else [[likely]]{
					trylog.debug(L"Executing Task from Thread");
					task_->Execute();
					hasJob = false;
					pMaster_->SignalDone();
				}
			}
		}
		Master* pMaster_;
		std::jthread thread_;
		std::condition_variable cv_;
		std::mutex mtx_;
		// Callback
		std::unique_ptr<Task> task_;
		
		// State
		bool dying = false;
		bool hasJob = false;
	};
}