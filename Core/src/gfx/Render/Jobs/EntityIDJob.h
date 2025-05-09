#pragma once
#include "IJob.h"
#include "Core/src/ecs/EntityID.h"

namespace tryn::gfx
{
	class EntityIDJob : public IJob
	{
		friend class RenderQueue;

	public:
		EntityIDJob(const class Model& model, const class glm::mat4& transform, ecs::EntityID entityID, class IRenderGraph& renderGraph, std::uint16_t jobID = 0);
		void Execute(const IContext& gfx) override;
		void Execute(const IGraphics& gfx) override;

	private:
		Model const* pModel;
		glm::mat4 const* pTransform;
		ecs::EntityID entityID;
		IRenderGraph* pRenderGraph;
	};
}
