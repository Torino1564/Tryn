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
		Worker(Master* pMaster);
		void StartWorking();
		void StopWorking();
		void SetTask(std::unique_ptr<Task>&& task);
		void Kill();
	protected:
		void WorkerKernel_();
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