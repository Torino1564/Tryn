#include "RenderTask.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	void RenderTask::operator()()
	{
		params.pDrawable->BindBase(*params.pContext);
		params.pDrawable->BindTransformCBuf(*params.pContext);
		params.pStep->Bind(*params.pGfx, *params.pContext);
		params.pContext->DrawIndexed(params.pDrawable->GetIndexCount());
	}

	void BatchRenderTask::operator()()
	{
		for (std::vector<Job>::iterator it = params.begin; it != params.end; it++)
		{
			auto& data = it->GetData();
			auto pDrawable = data.pDrawable;
			auto pStep = data.pStep;

			pDrawable->BindBase(*params.pContext);
			pDrawable->BindTransformCBuf(*params.pContext);
			pStep->Bind(*params.pGfx, *params.pContext);
			params.pContext->DrawIndexed(pDrawable->GetIndexCount());
		}
	}

}
