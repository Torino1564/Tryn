#include "TrynPCH.h"
#include "GenericTaskQueue.h"

namespace tryn::ccr
{
	bool GenericTaskQueue::Empty() const
	{
		return tasks_.empty();
	}
	void GenericTaskQueue::PushTask_(Task&& task)
	{
		std::lock_guard lck{ mtx_ };
		tasks_.push_back(std::move(task));
	}
}