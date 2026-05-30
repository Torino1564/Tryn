#pragma once
#include "Bindable.h"
#include <Core/src/spa/Dimensions.h>
#include <memory>
#include <Core/src/gfx/TextureFormat.h>

#include "DepthStencil.h"

namespace tryn::gfx
{
	class ITexture;
	class IDepthStencil;
	class IRenderTargetView : public IBindable
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, uint16_t rtvSlot, TextureFormat format = TextureFormat::B8G8R8A8_UNORM, uint16_t textureSlot = 0);
		static std::shared_ptr<IRenderTargetView> Resolve(const IGraphics& gfx, spa::DimensionsI dimensions, uint16_t rtvSlot, TextureFormat format = TextureFormat::B8G8R8A8_UNORM, uint16_t textureSlot = 0);
		void SetSlot(uint16_t slot);
		uint16_t GetSlot() const;
		virtual void Clear() const = 0;
		virtual void RegenerateResources(const spa::DimensionsI dimensions) = 0;
		void SetDepthStencil(IDepthStencil& dsv);
		ITexture& GetTexture();
		const ITexture& GetTexture() const;
		TextureFormat GetFormat() const;
		spa::DimensionsI GetDimensions() const;
	protected:
		std::shared_ptr<ITexture> pTexture;
		IDepthStencil* pDSV = nullptr;
		uint16_t slot = 0;
		uint16_t textureSlot = 0;
		spa::DimensionsI dimensions = {};
		TextureFormat format;
	};
}
