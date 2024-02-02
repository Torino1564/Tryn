#include "Step.h"
#include <Core/src/gfx/Model/Mesh.h>
#include <Core/src/gfx/Render/TechniqueProbe.h>
#include <Core/src/gfx/Model/InstancedModel.h>

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
	void Step::Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, InstancedModelParent& instanceParent)
	{
		auto& renderGraph = gfx.GetRenderGraph();
		renderGraph.GetRenderQueueByID(renderQueueID).Push(Job(parent, this, transforms, &instanceParent));
	}
	void Step::Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);
		for (auto& pBindable : bindables)
		{
			pBindable->Accept(probe);
		}
	}
}