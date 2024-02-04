#pragma once
#include <stdint.h>
#include "SinkAndSource.h"
#include <memory>
#include <vector>
#include <unordered_map>

#define ZT_DEFINE_RENDER_PASS(x) class x : public tryn::gfx::BaseRenderPass<x>

namespace tryn::gfx
{
	class RenderQueue;

	class IRenderPass
	{
	public:
		virtual void Execute(IGraphics& gfx) = 0;

		struct RenderPassID
		{
			static uint16_t Resolve()
			{
				static uint16_t UIDcounter = 0;
				return UIDcounter++;
			}
		};
	};

	template <typename T>
	class BaseRenderPass : public IRenderPass
	{
	protected:
		// resources
		ISink* pSink;
		ISource* pSource;

		// queues
		std::vector<RenderQueue*> pQueues;
		std::vector<std::string> queueNames;

		// id
		static inline const uint16_t ID = RenderPassID::Resolve();
	};
}