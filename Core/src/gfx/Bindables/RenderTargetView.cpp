#include "TrynPCH.h"
#include "RenderTargetView.h"
#include <Core/src/gfx/BindablePool.h>

#include "Core/src/utl/TypeName.h"

namespace tryn::gfx 
{
	void IGenericRenderTargetView::BindAsRTV(IGenericDepthStencil* pDSV)
	{
		trylog.warn(L"BindAsRTV no implementation found!");
	}

	template <BufferResourceType Type>
	std::string IRenderTargetView<Type>::GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, const uint16_t slot, const RenderTargetFormat format)
		requires (Type == BufferResourceType::ShaderResource)
	{
			static uint16_t rtvCounter = 0u;
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#";
			UID += "SR";
			UID += "RTV#W:";
			UID += dimensions.width;
			UID += "#H:";
			UID += dimensions.height;
			UID += "#";
			UID += "Slot:";
			UID += slot;
			UID += "F:";
			UID += std::to_underlying(format);
			UID += "#";
			UID += rtvCounter++;

			return UID;
	}

	template <BufferResourceType Type>
	std::string IRenderTargetView<Type>::GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, const RenderTargetFormat format)
		requires (Type == BufferResourceType::OutputOnly)
	{
		static uint16_t rtvCounter = 0u;
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#";
		UID += "OO";
		UID += "RTV#W:";
		UID += dimensions.width;
		UID += "#H:";
		UID += dimensions.height;
		UID += "F:";
		UID += std::to_underlying(format);
		UID += "#";
		UID += rtvCounter++;

		return UID;
	}

	template <BufferResourceType Type>
	std::shared_ptr<IShaderResourceRenderTargetView> IRenderTargetView<Type>::Resolve(const IGraphics& gfx,
		const spa::DimensionsI dimensions, uint16_t slot, const RenderTargetFormat format) requires (Type == BufferResourceType::ShaderResource)
	{
		return BindablePool::Resolve<IShaderResourceRenderTargetView>(gfx, dimensions, slot, format);
	}

	template <BufferResourceType Type>
	std::shared_ptr<IOutputOnlyRenderTargetView> IRenderTargetView<Type>::Resolve(const IGraphics& gfx,
		const spa::DimensionsI dimensions, const RenderTargetFormat format) requires (Type == BufferResourceType::OutputOnly)
	{
		return BindablePool::Resolve<IOutputOnlyRenderTargetView>(gfx, dimensions, format);
	}

	template class IRenderTargetView<BufferResourceType::OutputOnly>;
	template class IRenderTargetView<BufferResourceType::ShaderResource>;
}
