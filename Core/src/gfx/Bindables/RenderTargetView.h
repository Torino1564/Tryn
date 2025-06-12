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
		static std::string GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format = TextureFormat::B8G8R8A8_UNORM);
		// The optional slot parameter is unused if the shaderResource flag is set to false. 
		static std::shared_ptr<IRenderTargetView> Resolve(const IGraphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format = TextureFormat::B8G8R8A8_UNORM);

		virtual void BindAsRTV(IDepthStencil* pDSV = nullptr) const = 0;
		virtual void Clear() const = 0;
		virtual void FillTextureRegion(const std::shared_ptr<ITexture>&, uint32_t startX, uint32_t endX, uint32_t startY, uint32_t endY) = 0;
		virtual void FillTexture(const std::shared_ptr<ITexture>&) = 0;

	protected:
		spa::DimensionsI dimensions = {};
		bool shaderResource = true;
		TextureFormat format = TextureFormat::B8G8R8A8_UNORM;
		uint16_t slot = 0;
	};
}
