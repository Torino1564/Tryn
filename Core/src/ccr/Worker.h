#pragma once
#include "Thread.h"

namespace tryn::ccr
{
	class Master;

	class Worker : public Thread
	{
		using Task = ccr::GenericTaskQueue::Task;
	public:
		Worker() : pMaster_(nullptr) {}
		virtual ~Worker() = default;
		Worker(Master* pMaster);
		void AfterAllTasks() override;
	protected:
		Master* pMaster_;
	};
}