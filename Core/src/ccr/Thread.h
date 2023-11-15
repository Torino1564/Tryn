#pragma once
#include <future>
#include <thread>
#include "GenericObjectPool.h"

namespace tryn::ccr
{
	class Thread
	{
		class Task
		{
		public:
			virtual void Execute() = 0;
		};
		void SetTask(ccr::GenericUniqueReference<Task>&& task);
		void StartWorking();
		void Kill();
	protected:
		virtual void PostWork();
		void WorkerKernel_();
		std::jthread thread_;
		std::condition_variable cv_;
		std::mutex mtx_;
		// Callback
		ccr::GenericUniqueReference<Task> task_;
		// State
		bool dying = false;
		bool hasJob = false;
	};
}