
#include "RenderQueuePass.h"
#include <Core/src/gfx/Render/RenderGraph.h>

namespace tryn::gfx
{
	RenderQueuePass::RenderQueuePass(RenderQueuePass&& rhs) noexcept
		:
		IRenderPass(std::move(rhs))
	{
		pQueues = std::move(rhs.pQueues);
		queueNames = std::move(rhs.queueNames);
	}
	RenderQueuePass::RenderQueuePass(std::string name, IRenderGraph& graph, std::vector<std::string> queueNames)
		:
		IRenderPass(std::move(name))
	{
		for (auto& queueID : queueNames)
		{
			pQueues.emplace_back(&graph.GetOrAddRenderQueue(queueID));
			this->queueNames.push_back(queueID);
		}
	}
}