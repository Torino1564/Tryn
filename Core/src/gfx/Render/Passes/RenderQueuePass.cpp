#include "RenderQueuePass.h"
#include <Core/src/gfx/Render/RenderGraph.h>

namespace tryn::gfx
{
	RenderQueuePass::RenderQueuePass(std::string name, IRenderGraph& graph, std::vector<std::string> queueNames)
		:
		IRenderPass(std::move(name)), queueVector(graph.GetQueueVector())
	{
		for (auto& queueID : queueNames)
		{
			queueIndeces.emplace_back(graph.GetOrAddRenderQueueID(queueID));
			this->queueNames.push_back(queueID);
		}
	}
}