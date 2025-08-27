
#include "Thread.h"

namespace tryn::ccr
{
	void Thread::ThreadKernel_()
	{
		std::unique_lock lock(mtx_);
		while (true)
		{
			cv_.wait(lock, [this] {
				return !halted && hasWork || dying;
				});
			if (dying)
			{
				break;
			}
			else [[likely]]
			{
				while (!tasks_.Empty())
				{
					tasks_.PopExecute();
					AfterTask();
				}
				hasWork = false;
				AfterAllTasks();
			}
		}
	}
}
