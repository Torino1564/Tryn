
#include "RenderTargetView.h"
#include <Core/src/gfx/BindablePool.h>

#include "TextureResource.h"

namespace tryn::gfx
{
	std::string IRenderTargetView::GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, const uint16_t rtvSlot, const TextureFormat format, const uint16_t textureSlot)
	{
		using namespace std::string_literals;

		auto& typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#";
		UID += "RTV#W:";
		UID += dimensions.width;
		UID += "#H:";
		UID += dimensions.height;
		UID += "#Slot:" + std::to_string(rtvSlot);
		UID += "#Tex#Slot:";
		UID += textureSlot;
		UID += "#F:";
		UID += std::to_underlying(format);
		UID += "#";

		return UID;
	}

	void IRenderTargetView::SetSlot(const uint16_t slot)
	{
		this->slot = slot;
	}

	uint16_t IRenderTargetView::GetSlot() const
	{
		return slot;
	}

	std::shared_ptr<IRenderTargetView> IRenderTargetView::Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, const uint16_t rtvSlot, const TextureFormat format, const uint16_t textureSlot)
	{
		return BindablePool::Resolve<IRenderTargetView>(gfx, dimensions, rtvSlot, format, textureSlot);
	}

	void IRenderTargetView::SetDepthStencil(IDepthStencil& dsv)
	{
		pDSV = &dsv;
	}

	ITexture& IRenderTargetView::GetTexture()
	{
		return *pTexture;
	}

	const ITexture& IRenderTargetView::GetTexture() const
	{
		return *pTexture;
	}

	TextureFormat IRenderTargetView::GetFormat() const
	{
		return pTexture->GetFormat();
	}

	spa::DimensionsI IRenderTargetView::GetDimensions() const
	{
		return pTexture->GetDimensions();
	}
}
