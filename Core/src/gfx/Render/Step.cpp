#include "TrynPCH.h"
#include "Step.h"
#include <Core/src/gfx/Model/Mesh.h>
#include <Core/src/gfx/Render/TechniqueProbe.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/Render/Jobs/BasicJob.h>
#include <Core/src/gfx/Render/Jobs/InstancedJob.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	Step::Step(const std::string& renderQueueID)
		:
		renderQueueID(renderQueueID)
	{
	}
	void Step::AddBindable(std::shared_ptr<IBindable> bindable)
	{
		bindables.push_back(std::move(bindable));
	}
	void Step::Bind() const
	{
		for (auto& bind : bindables)
		{
			bind->Bind();
		}
	}
	void Step::Bind(const IContext& context) const
	{
		for (auto& bind : bindables)
		{
			bind->Bind(context);
		}
	}
	void Step::Draw(const IGraphics& gfx, const Drawable* parent)
	{
		gfx.DrawIndexed(parent->GetIndexCount());
	}

	void Step::OfferBindable(const std::string& identifier, const std::shared_ptr<IBindable>& pBindable)
	{
		if (const auto it = std::ranges::find(bindablesToAccept, identifier); it != bindablesToAccept.end())
		{
			inheritedBindables.push_back(pBindable);
		}
	}

	void Step::Submit(const IGraphics& gfx, Drawable* parent)
	{
		auto& renderGraph = gfx.GetRenderGraph();
		renderGraph.GetRenderQueueByID(renderQueueID).Push(BasicJob(parent,this));
	}
	void Step::Submit(const IGraphics& gfx, Drawable* parent, const std::span<const glm::mat4> transforms, InstancedModelParent& instanceParent)
	{
		auto& renderGraph = gfx.GetRenderGraph();
		renderGraph.GetRenderQueueByID(renderQueueID).Push(InstancedJob(parent, this, transforms, &instanceParent));
	}
	void Step::Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);
		for (auto& pBindable : bindables)
		{
			pBindable->Accept(probe);
		}
	}

	const std::string& Step::RenderQueueID() const
	{
		return renderQueueID;
	}
}
