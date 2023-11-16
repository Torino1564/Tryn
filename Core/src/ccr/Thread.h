#pragma once
#include <future>
#include <thread>
#include "GenericTaskQueue.h"

namespace tryn::ccr
{
	class Thread
	{
	public:
		virtual ~Thread() = default;
		template <std::invocable InputTask>
		void AddTask(std::unique_ptr<InputTask>&& function)
		{
			tasks_.Push(std::forward<std::unique_ptr<InputTask>&&>(function));
			{
				std::lock_guard lk(mtx_);
				hasWork = true;
			}
			cv_.notify_one();
		}
		template <std::invocable InputTask>
		void AddTask(InputTask* function)
		{
			tasks_.Push(function);
			{
				std::lock_guard lk(mtx_);
				hasWork = true;
			}
			cv_.notify_one();
		}
		void Halt()
		{
			{
				std::lock_guard lk(mtx_);
				halted = true;
			}
			cv_.notify_one();
		}
		void Resume()
		{
			{
				std::lock_guard lk(mtx_);
				halted = false;
			}
			cv_.notify_one();
		}
		void StartWorking()
		{
			thread_ = std::jthread(&ccr::Thread::ThreadKernel_, this);
			Resume();
		}
		void Kill()
		{
			{
				std::lock_guard lk(mtx_);
				dying = true;
			}
			cv_.notify_one();
		}
		virtual void AfterTask() {};
		virtual void AfterAllTasks() {};
	protected:
		void ThreadKernel_();
		std::jthread thread_;
		std::condition_variable cv_;
		std::mutex mtx_;

		//Work
		ccr::GenericTaskQueue tasks_;

		//State
		bool halted = true;
		bool hasWork = false;
		bool dying = false;
	};
}