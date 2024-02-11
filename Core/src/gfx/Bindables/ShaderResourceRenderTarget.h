#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>

namespace tryn::gfx
{
	class IShaderResourceRenderTarget : public IBindable
	{
	public:
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
		{
			static uint16_t srrtv = 0u;

			auto out = std::format("{}#DS#W:{}H:{}#Slot:#{}",
				IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
				dimensions.width, dimensions.height, slot, srrtv++
			);

			return out;
		}
		static std::shared_ptr<IShaderResourceRenderTarget> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot);
	};
}