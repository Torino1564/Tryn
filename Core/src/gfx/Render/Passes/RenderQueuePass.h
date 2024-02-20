#pragma once
#include <Core/src/gfx/Render/RenderPass.h>

namespace tryn::gfx
{
	class RenderQueue;
	class RenderQueuePass : public IRenderPass
	{
	public:
		RenderQueuePass(std::string name)
			:
			IRenderPass(std::move(name))
		{

		}
	protected:
		// queues
		std::vector<RenderQueue> queues;
		std::vector<std::string> queueNames;
	};
}