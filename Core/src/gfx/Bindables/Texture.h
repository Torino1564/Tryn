#pragma once
#include "Bindable.h"
#include <Core/src/gfx/BindablePool.h>
#include <filesystem>
#include <Core/src/gfx/Texture.h>

namespace tryn::gfx
{
	class ITexture : public IBindable
	{
	public:
		static std::string GenerateID(IGraphics& gfx, const std::filesystem::path path, uint8_t slot)
		{
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#Texture#";
			UID += path.string();
			UID += "#";
			UID += std::to_string(slot);

			return UID;
		}
		static std::shared_ptr<ITexture> Resolve(IGraphics& gfx, const std::filesystem::path path, uint8_t slot)
		{
			return BindablePool::Resolve<ITexture>(gfx, path, slot);
		}
		bool HasAlpha() const
		{
			return hasAlpha;
		}
	protected:
		std::string path;
		uint8_t slot = 0;
		bool hasAlpha = false;
	};
}