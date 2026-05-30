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
		RenderQueuePass(RenderQueuePass&& rhs) noexcept;
		RenderQueuePass(std::string name, IRenderGraph& graph, std::vector<std::string> queueNames );
	protected:

		// queues
		std::vector<RenderQueue*> pQueues;
		std::vector<std::string> queueNames;
	};
}