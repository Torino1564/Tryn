#include "RenderTask.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	void tryn::gfx::RenderTask::operator()()
	{
		params.pDrawable->BindBase(*params.pContext);
		params.pStep->Bind(*params.pGfx, *params.pContext);
		params.pContext->DrawIndexed(params.pDrawable->GetIndexCount());
	}
}
