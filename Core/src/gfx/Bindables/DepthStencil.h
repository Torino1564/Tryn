#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <format>
#include <Core/src/gfx/ComparissonMode.h>

namespace tryn::gfx
{
	class IDepthStencil : public IBindable
	{
	public:

		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less)
		{
			static uint16_t depthStencil = 0u;

			auto out = std::format("{}#DS#W:{}H:{}#Mode:{}#{}",
				IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
				dimensions.width, dimensions.height, (int)mode, depthStencil++
				);

			return out;
		}
		static std::shared_ptr<IDepthStencil> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less);

	};
}