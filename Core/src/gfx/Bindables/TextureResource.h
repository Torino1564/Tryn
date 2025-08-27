#pragma once
#include "Bindable.h"
#include <Core/src/spa/Dimensions.h>
#include <Core/src/gfx/Texture.h>

struct aiTexture;

namespace tryn::gfx
{
	class ITexture : public IBindable
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage = TextureUsage::GPUOnly);
		static std::string GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, TextureFormat format, uint8_t slot, TextureUsage usage = TextureUsage::GPUOnly);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage = TextureUsage::GPUOnly);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, spa::DimensionsI dimensions, TextureFormat format, uint8_t slot, TextureUsage usage = TextureUsage::GPUOnly);
		bool HasAlpha() const;
		const Texture& GetTextureResource() const;
		const std::string& GetPath() const;
		TextureUsage GetUsage() const;
		TextureFormat GetFormat() const;
		spa::DimensionsI GetDimensions() const;
		void SetSlot(uint16_t slot);
		uint16_t GetSlot() const;
		
	protected:

		std::shared_ptr<Texture> pTextureResource;
		TextureFormat format = TextureFormat::B8G8R8A8_UNORM;
		TextureUsage usage = TextureUsage::GPUOnly;
		spa::DimensionsI dimensions = {};
		uint8_t slot = 0;
		bool hasAlpha = false;
	};
}