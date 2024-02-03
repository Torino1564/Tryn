#include "RenderTask.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	void RenderTask::operator()()
	{
		params.pJob->Execute(*params.pContext);
	}

	void BatchRenderTask::operator()()
	{
		for (std::vector<IJob*>::iterator it = params.begin; it != params.end; it++)
		{
			(*(*(it._Ptr))).Execute(*params.pContext);
		}
	}

}
