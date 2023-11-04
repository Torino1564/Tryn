#pragma once
#include <thread>
#include <future>
#include <span>
#include <functional>
#include <tuple>

namespace tryn::ccr
{
	class Master
	{
	public:
		Master(int workerCount)
			:
			lock(mtx), workerCount(workerCount), doneCount(0)
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
		int GetWorkerCount() const
		{
			return workerCount;
		}
	private:
		std::condition_variable cv;
		std::mutex mtx;
		std::unique_lock<std::mutex> lock;
		int workerCount;
		//Shared memory
		int doneCount = 0;
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
		template<std::invocable<Args...> Callback>
		void SetJob(Callback task, Args ... params)
		{
			{
				std::lock_guard lk(mtx_);
				task_ = task;
				parameters_ = std::make_tuple(params...);
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
				std::apply(task_,parameters_);
				hasJob = false;
				pMaster_->SignalDone();
			}
		}
		Master* pMaster_;
		std::jthread thread_;
		std::condition_variable cv_;
		std::mutex mtx_;
		// Callback
		std::function<void(Args...)> task_;
		std::tuple<Args...> parameters_;
		
		// State
		bool dying = false;
		bool hasJob = false;
	};
}