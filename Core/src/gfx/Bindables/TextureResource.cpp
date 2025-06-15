#include "TrynPCH.h"
#include "TextureResource.h"
#include <Core/src/gfx/BindablePool.h>
#include <assimp/texture.h>

namespace tryn::gfx
{
	std::string ITexture::GenerateID(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#Texture#";
		UID += pTexture->GetPath();
		UID += "#S:";
		UID += std::to_string(slot);
		UID += "#U:";
		UID += to_string(usage);
		return UID;
	}

	std::shared_ptr<ITexture> ITexture::Resolve(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage)
	{
		return BindablePool::Resolve<ITexture>(gfx, std::move(pTexture), slot, usage);
	}

	bool ITexture::HasAlpha() const
	{
		return pTextureResource->HasAlpha();
	}

	const Texture& ITexture::GetTextureResource() const
	{
		return *pTextureResource;
	}

	const std::string& ITexture::GetPath() const
	{
		return pTextureResource->GetPath();
	}

	TextureUsage ITexture::GetUsage() const
	{
		return usage;
	}

	void ITexture::SetSlot(uint16_t slot)
	{
		this->slot = slot;
	}
}
