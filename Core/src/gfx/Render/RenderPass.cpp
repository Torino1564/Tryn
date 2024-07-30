#include "TrynPCH.h"
#include <Core/src/gfx/Render/RenderPass.h>

namespace tryn::gfx
{
	IRenderPass::IRenderPass(std::string name): name(std::move(name))
	{}

	IRenderPass::IRenderPass(IRenderPass&& rhs) noexcept
	{
		pSink = std::move(rhs.pSink);
		pSource = std::move(rhs.pSource);
		name = std::move(rhs.name);
	}

	const std::string& IRenderPass::GetName() const
	{
		return name;
	}

	uint16_t IRenderPass::RenderPassID::Resolve()
	{
		static uint16_t UIDcounter = 0;
		return UIDcounter++;
	}

	ISink& IRenderPass::GetSink() const
	{
		return *pSink;
	}

	ISource& IRenderPass::GetSource() const
	{
		return *pSource;
	}
}
