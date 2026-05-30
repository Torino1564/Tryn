
#include "BasicJob.h"
#include <Core/src/gfx/Drawable.h>
#include <Core/src/gfx/Render/Step.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	BasicJob::BasicJob(Drawable* parent, Step* step)
		:
		data{ parent,step }
	{}

	void BasicJob::Execute(const IGraphics& gfx)
	{
		data.pDrawable->BindBase();
		data.pDrawable->BindExtraBinds();
		data.pDrawable->BindTransformCBuf();
		data.pStep->Bind();
		gfx.DrawIndexed(data.pDrawable->GetIndexCount());
	}

	void BasicJob::Execute(const IContext& ctx)
	{
		data.pDrawable->BindBase(ctx);
		data.pDrawable->BindTransformCBuf(ctx);
		data.pStep->Bind(ctx);
		ctx.DrawIndexed(data.pDrawable->GetIndexCount());
	}
	BasicJob::Data& BasicJob::GetData()
	{
		return data;
	}
}