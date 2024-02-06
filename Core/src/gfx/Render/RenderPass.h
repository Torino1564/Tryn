#pragma once
#include <stdint.h>
#include "SinkAndSource.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace tryn::gfx
{
	class RenderQueue;

	class IRenderPass
	{
	public:
		virtual void Execute(IGraphics& gfx) = 0;
		const std::string& GetName() const
		{
			return name;
		}
		struct RenderPassID
		{
			static uint16_t Resolve()
			{
				static uint16_t UIDcounter = 0;
				return UIDcounter++;
			}
		};
		ISink& GetSink()
		{
			return *pSink;
		}
		ISource& GetSource()
		{
			return *pSource;
		}
	protected:
		// resources
		std::unique_ptr<ISink> pSink;
		std::unique_ptr<ISource> pSource;

		// queues
		std::vector<RenderQueue*> pQueues;
		std::vector<std::string> queueNames;

		// identification
		std::string name;
		static inline const uint16_t ID = RenderPassID::Resolve();
	};
}