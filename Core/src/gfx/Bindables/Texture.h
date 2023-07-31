#pragma once
#include "Bindable.h"

namespace tryn::gfx
{
	class ITexture : public IBindable
	{
	public:
		static std::string GenerateUID(IGraphics& gfx, const std::string_view path, uint8_t slot)
		{
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#Texture#";
			UID += path;
			UID += "#";
			UID += std::to_string(slot);

			return UID;
		}
	protected:
		std::string path;
		uint8_t slot = 0;
		bool hasAlpha = false;
	};
}