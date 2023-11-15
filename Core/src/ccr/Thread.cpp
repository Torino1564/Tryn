#include "Thread.h"

namespace tryn::ccr
{
	void Thread::SetTask(ccr::GenericUniqueReference<Task>&& task)
	{
		{
			std::lock_guard lk(mtx_);
			task_ = std::move(task);
			hasJob = true;
		}
		cv_.notify_one();
	}
	inline void Thread::PostWork()
	{
	}
	void Thread::WorkerKernel_()
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
				task_->Execute();
				//Execute();
				hasJob = false;
				PostWork();
				}
		}
	}
	void Thread::StartWorking()
	{
		thread_ = std::jthread(&Thread::WorkerKernel_, this);
	}
	void Thread::Kill()
	{
		{
			std::lock_guard lk(mtx_);
			dying = true;
		}
		cv_.notify_one();
	}
}
