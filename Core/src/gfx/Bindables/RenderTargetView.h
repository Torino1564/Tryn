#pragma once
#include "Bindable.h"

namespace tryn::gfx
{
	class IRenderTargetView : public IBindable
	{
	public:
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions)
		{
			static uint16_t rtvCounter = 0u;
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#RTV#W:";
			UID += dimensions.width;
			UID += "#H:";
			UID += dimensions.height;
			UID += "#";
			UID += rtvCounter++;

			return UID;
		}
		static std::shared_ptr<IRenderTargetView> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions);
		
	};
}