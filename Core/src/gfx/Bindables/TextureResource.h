#pragma once
#include "Bindable.h"
#include <filesystem>
#include <Core/src/gfx/Texture.h>

struct aiTexture;

namespace tryn::gfx
{
	class ITexture : public IBindable
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage = TextureUsage::GPUOnly);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage = TextureUsage::GPUOnly);
		bool HasAlpha() const;
		const Texture& GetTextureResource() const;
		const std::string& GetPath() const;
		TextureUsage GetUsage() const;
		void SetSlot(uint16_t slot);
	protected:
		std::shared_ptr<Texture> pTextureResource;
		TextureUsage usage = TextureUsage::GPUOnly;
		uint8_t slot = 0;
		bool hasAlpha = false;
	};
}