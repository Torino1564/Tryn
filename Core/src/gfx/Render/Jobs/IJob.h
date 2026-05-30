#pragma once
#include <cstdint>

namespace tryn::gfx
{
	class IGraphics;
	class IContext;

	class IJob
	{
		friend class RenderQueue;
	public:
		IJob(std::uint16_t jobID = 0) : jobID(jobID) {}
		virtual ~IJob() = default;
		virtual void Execute(const IGraphics& gfx) = 0;
		virtual void Execute(const IContext& gfx) = 0;

	protected:
		std::uint16_t jobID = 0;
	};
}