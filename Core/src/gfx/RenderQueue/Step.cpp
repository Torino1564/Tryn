#include "Step.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	Step::Step(std::string renderQueueID)
		:
		renderQueueID(renderQueueID)
	{
	}
	void Step::AddBindable(std::shared_ptr<IBindable> bindable)
	{
		bindables.push_back(std::move(bindable));
	}
	void Step::Bind(IGraphics& gfx) const
	{
		for (auto& bind : bindables)
		{
			bind->Bind();
		}
	}
	void Step::Bind(IGraphics& gfx, IContext& context) const
	{
		for (auto& bind : bindables)
		{
			bind->Bind(context);
		}
	}
	void Step::Draw(IGraphics& gfx, Drawable* parent) const
	{
		gfx.DrawIndexed(parent->GetIndexCount());
	}
	void Step::Submit(IGraphics& gfx, Drawable* parent)
	{
		auto& renderGraph = gfx.GetRenderGraph();
		renderGraph.GetRenderQueueByID(renderQueueID).Push(Job(parent,this));
	}
}