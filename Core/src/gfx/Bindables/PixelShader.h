#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>

namespace tryn::gfx
{
	class IPixelShader : public IBindable
	{
	public:
		static const std::string GenerateID(IGraphics& gfx, std::string path)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#PixelShader#";
			UID += path;

			return UID;
		}
	};
}