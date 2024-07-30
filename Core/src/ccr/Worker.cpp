#include "TrynPCH.h"
#include "Worker.h"
#include "Master.h"

namespace tryn::ccr
{
	Worker::Worker(Master* pMaster)
		:
		pMaster_(pMaster)
	{}
	void Worker::AfterAllTasks()
	{
		pMaster_->SignalDone();
	}
}