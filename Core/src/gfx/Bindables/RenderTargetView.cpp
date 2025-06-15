#include "TrynPCH.h"
#include "RenderTargetView.h"
#include <Core/src/gfx/BindablePool.h>

#include "TextureResource.h"

namespace tryn::gfx 
{
	std::string IRenderTargetView::GenerateID(const IGraphics& gfx, const std::shared_ptr<ITexture>& pTexture, uint16_t slot)
	{
		static uint16_t rtvCounter = 0u;
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#";
		UID += "RTV#W:";
		UID += pTexture->GetTextureResource().GetWidth();
		UID += "#H:";
		UID += pTexture->GetTextureResource().GetHeight();
		if (shaderResource)
		{
			UID += "#SR#Slot:";
			UID += slot.value_or(0);
			if (!slot.has_value())
				trylog.warn(L"Render Target View marked as shader resource but binding slot not specified! Defaulting to slot 0.");
		}
		UID += "F:";
		UID += std::to_underlying(format);
		UID += "#";
		UID += rtvCounter++;

		return UID;
	}

	std::shared_ptr<IRenderTargetView> IRenderTargetView::Resolve(const IGraphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format)
	{
		return BindablePool::Resolve<IRenderTargetView>(gfx, dimensions, shaderResource, slot, format);
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
}
