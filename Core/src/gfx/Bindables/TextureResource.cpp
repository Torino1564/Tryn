#include "TrynPCH.h"
#include "TextureResource.h"
#include <Core/src/gfx/BindablePool.h>
#include <assimp/texture.h>

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

	std::string ITexture::GenerateID(const IGraphics& gfx, const aiTexture& tex, uint8_t slot)
	{
		return GenerateID(gfx, tex.mFilename.C_Str(), slot);
	}

	std::shared_ptr<ITexture> ITexture::Resolve(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot)
	{
		return BindablePool::Resolve<ITexture>(gfx, path, slot);
	}

	std::shared_ptr<ITexture> ITexture::Resolve(const IGraphics& gfx, const aiTexture& tex, uint8_t slot)
	{
		return BindablePool::Resolve<ITexture>(gfx, tex, slot);
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
