#pragma once
#include <thread>
#include <future>
#include <span>
#include <functional>

namespace tryn::ccr
{
	class Master
	{
	public:
		Master(int workerCount)
			:
			lock(mtx), workerCount(workerCount)
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
			cv.wait(lock, [this] {return doneCount == workerCount; });
			doneCount = 0;
		}
	private:
		std::condition_variable cv;
		std::mutex mtx;
		std::unique_lock<std::mutex> lock;
		int workerCount;
		//Shared memory
		int doneCount;
	};

	template<class ... Args>
	class Worker
	{
	public:
		Worker() : pMaster_(nullptr) {}
		Worker(Master* pMaster)
			:
			pMaster_(pMaster)
		{}
		void StartWorking()
		{
			thread_ = std::jthread(&Worker::WorkerKernel_, this);
		}
		void SetJob(std::invocable<void(Args)> auto task, Args ... params)
		{
			{
				std::lock_guard lk(mtx_);
				task_ = task;
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
				task_(parameters);
				hasJob = false;
				pMaster_->SignalDone();
			}
		}
		Master* pMaster_;
		std::jthread thread_;
		std::condition_variable cv_;
		std::mutex mtx_;
		// Callback
		std::function<void(Args)> task_;
		Args... parameters;
		// State
		bool dying = false;
		bool hasJob = false;
	};
}