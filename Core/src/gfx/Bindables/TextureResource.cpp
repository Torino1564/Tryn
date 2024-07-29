#include "TextureResource.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::string ITexture::GenerateID(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#Texture#";
		UID += path.string();
		UID += "#";
		UID += std::to_string(slot);

		return UID;
	}

	std::shared_ptr<ITexture> ITexture::Resolve(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot)
	{
		return BindablePool::Resolve<ITexture>(gfx, path, slot);
	}

	bool ITexture::HasAlpha() const
	{
		return pTextureResource->HasAlpha();
	}

	const Texture& ITexture::GetTextureResource() const
	{
		return *pTextureResource;
	}
}
