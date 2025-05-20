#include "TrynPCH.h"
#include "Step.h"
#include <Core/src/gfx/Model/Mesh.h>
#include <Core/src/gfx/Render/TechniqueProbe.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/Render/Jobs/BasicJob.h>
#include <Core/src/gfx/Render/Jobs/InstancedJob.h>
#include <Core/src/gfx/IGraphics.h>

#include "Core/src/gfx/Bindables/SOAVertexBuffer.h"
#include "Core/src/gfx/Bindables/VertexShader.h"

namespace tryn::gfx
{
	Step::Step(const IGraphics& gfx, const std::string& renderQueueID)
		:
		renderQueueID(renderQueueID)
	{
		pVertexLayout = std::make_unique<VertexLayout>();
		pSOAVertexBuffer = gfx.CreateSOAVertexBuffer();
	}

	Step::Step(Step&& rhs) noexcept
		:
		bindables(std::move(rhs.bindables)),
		acceptedBindables(rhs.acceptedBindables),
		pVertexShader(std::move(rhs.pVertexShader)),
		pVertexLayout(std::move(rhs.pVertexLayout)),
		pSOAVertexBuffer(std::move(rhs.pSOAVertexBuffer)),
		bindablesToAccept(std::move(rhs.bindablesToAccept)),
		renderQueueID(std::move(rhs.renderQueueID))
	{
	}

	Step::~Step() = default;

	void Step::AddBindable(const std::shared_ptr<IBindable>& bindable)
	{
		bindables.push_back(std::move(bindable));
	}

	void Step::AddBindable(const std::shared_ptr<IVertexShader>& pVS)
	{
		pVertexShader = pVS;
		pSOAVertexBuffer->SetVertexShader(pVS);
		bindables.push_back(pVS);
	}

	void Step::Bind() const
	{
		for (auto& bind : bindables)
		{
			bind->Bind();
		}
		for (auto& bind : acceptedBindables)
		{
			bind->Bind();
		}
		pSOAVertexBuffer->Bind();
	}
	void Step::Bind(const IContext& context) const
	{
		for (auto& bind : bindables)
		{
			bind->Bind(context);
		}
		for (auto& bind : acceptedBindables)
		{
			bind->Bind(context);
		}
		pSOAVertexBuffer->Bind(context);
	}
	void Step::Draw(const IGraphics& gfx, const Drawable* parent)
	{
		gfx.DrawIndexed(parent->GetIndexCount());
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

	void Step::OfferBindable(const std::string& identifier, const std::shared_ptr<IBindable>& pBindable)
	{
		if (const auto it = std::ranges::find(bindablesToAccept, identifier); it != bindablesToAccept.end())
		{
			acceptedBindables.push_back(pBindable);
		}
	}

	void Step::FillSOAVertexBuffer(const ISOAVertexBuffer& SOAVertexBuffer) const
	{
		pSOAVertexBuffer->AppendFrom(SOAVertexBuffer, *pVertexLayout);
	}

	const VertexLayout& Step::GetVertexLayout() const
	{
		return *pVertexLayout;
	}

	VertexLayout& Step::GetVertexLayout()
	{
		return *pVertexLayout;
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
