#include "RenderQueuePass.h"
#include <Core/src/gfx/Render/RenderGraph.h>

namespace tryn::gfx
{
	RenderQueuePass::RenderQueuePass(std::string name, IRenderGraph& graph, std::vector<std::string> queueNames)
		:
		IRenderPass(std::move(name))
	{
		for (auto& queueID : queueNames)
		{
			pQueues.emplace_back(&graph.GetOrAddRenderQueue(queueID));
			queueNames.push_back(queueID);
		}
	}
}