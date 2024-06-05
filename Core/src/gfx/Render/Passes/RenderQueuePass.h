#pragma once
#include <Core/src/gfx/Render/RenderPass.h>

namespace tryn::gfx
{
	class RenderQueue;
	class IRenderGraph;

	class RenderQueuePass : public IRenderPass
	{
	public:
		virtual ~RenderQueuePass() = default;
		RenderQueuePass(RenderQueuePass&& rhs) noexcept
			:
			IRenderPass(std::move(rhs)), queueVector(rhs.queueVector)
		{
			queueIndeces = std::move(rhs.queueIndeces);
			queueNames = std::move(rhs.queueNames);
		}
		RenderQueuePass(std::string name, IRenderGraph& graph, std::vector<std::string> queueNames );
	protected:
		// queues
		std::vector<uint16_t> queueIndeces;
		std::vector<std::string> queueNames;
		std::vector<gfx::RenderQueue>& queueVector;
	};
}