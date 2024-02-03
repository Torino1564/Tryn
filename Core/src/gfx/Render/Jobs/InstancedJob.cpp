#include "InstancedJob.h"
#include <Core/src/gfx/Drawable.h>
#include <Core/src/gfx/Render/Step.h>

tryn::gfx::InstancedJob::InstancedJob(Drawable* parent, Step* step, std::span<const glm::mat4> transforms, InstancedModelParent* pParentInstanced)
	:
	data {.pDrawable = parent,  .pStep = step, .transforms = transforms, .instanceParent = pParentInstanced}
{
}

void tryn::gfx::InstancedJob::Execute(IGraphics& gfx)
{
	data.pDrawable->BindBase();
	data.pDrawable->BindExtraBinds();
	data.pDrawable->BindTransformCBuf();
	data.pStep->Bind();
	gfx.DrawIndexedInstanced(data.pDrawable->GetIndexCount(), data.transforms.size(), 0u, 0u, 0u);
}

void tryn::gfx::InstancedJob::Execute(IContext& ctx)
{
	data.pDrawable->BindBase(ctx);
	data.pDrawable->BindExtraBinds(ctx);
	data.pDrawable->BindTransformCBuf(ctx);
	data.pStep->Bind(ctx);
	ctx.DrawIndexedInstanced(data.pDrawable->GetIndexCount(), data.transforms.size(), 0u, 0u, 0u);
}
