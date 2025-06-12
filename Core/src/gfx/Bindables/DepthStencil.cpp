#include "TrynPCH.h"
#include "DepthStencil.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::string IDepthStencil::GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, const bool shaderResource, const std::optional<uint16_t> slot, const ComparissonMode mode)
	{
		static uint16_t depthStencil = 0u;

		auto out = std::format("{}#DS#W:{}H:{}{}#Mode:{}#{}",
			IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
			dimensions.width, dimensions.height,shaderResource ? "#SR#S:" + std::to_string(slot.value_or(0)):"", to_string(mode), depthStencil++
			);

		return out;
	}

	std::shared_ptr<IDepthStencil> IDepthStencil::Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, ComparissonMode mode)
	{
		return BindablePool::Resolve<IDepthStencil>(gfx, dimensions, shaderResource, slot, mode);
	}

	bool IDepthStencil::IsShaderResource() const
	{
		return shaderResource;
	}
}
