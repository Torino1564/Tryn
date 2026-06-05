
#include "RenderTask.h"
#include <Core/src/gfx/Drawable.h>
#include <Core/src/gfx/Render/Jobs/IJob.h>

namespace tryn::gfx
{
	void RenderTask::operator()() const
	{
		params.pJob->Execute(*params.pContext);
	}

	void BatchRenderTask::operator()() const
	{
		for (auto it = params.begin; it != params.end; ++it)
		{
			auto pIJob = *it;
			pIJob->Execute(*params.pContext);
		}
	}

}
