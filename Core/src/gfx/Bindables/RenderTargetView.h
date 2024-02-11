#pragma once
#include "Bindable.h"
#include <concepts>

namespace tryn::gfx
{
	enum class RTType
	{
		OutputOnly,
		ShaderResource
	};

	template <RTType Type>
	class IRenderTargetView;

	using IShaderResourceRenderTargetView = IRenderTargetView<RTType::ShaderResource>;
	using IOutputOnlyRenderTargetView = IRenderTargetView<RTType::OutputOnly>;

	template <RTType Type>
	class IRenderTargetView : public IBindable
	{
	public:
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == RTType::ShaderResource)
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
			UID += "#";
			UID += rtvCounter++;

			return UID;
		}
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == RTType::OutputOnly)
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
			UID += "#";
			UID += rtvCounter++;

			return UID;
		}
		static std::shared_ptr<IShaderResourceRenderTargetView> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == RTType::ShaderResource)
		{
			return BindablePool::Resolve<IShaderResourceRenderTargetView>(gfx, dimensions, slot);
		}
		static std::shared_ptr<IOutputOnlyRenderTargetView> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == RTType::OutputOnly)
		{
			return BindablePool::Resolve<IOutputOnlyRenderTargetView>(gfx, dimensions);
		}
		
	};
}