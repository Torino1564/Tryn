
#include "RenderTask.h"
#include <Core/src/gfx/Drawable.h>

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
			(*it._Ptr)->Execute(*params.pContext);
		}
	}

}
