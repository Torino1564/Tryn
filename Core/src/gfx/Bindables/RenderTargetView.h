#pragma once
#include "Bindable.h"
#include <concepts>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/gfx/Bindables/BufferResourceType.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	template <BufferResourceType Type = BufferResourceType::OutputOnly>
	class IRenderTargetView : public IBindable
	{
	public:
		template <BufferResourceType Type = Type>
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
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
			UID += "#";
			UID += rtvCounter++;

			return UID;
		}
		template <BufferResourceType Type = Type>
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions)
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
			UID += "#";
			UID += rtvCounter++;

			return UID;
		}

		template <BufferResourceType Type = Type>
		static std::shared_ptr<IShaderResourceRenderTargetView> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource)
		{
			return BindablePool::Resolve<IShaderResourceRenderTargetView>(gfx, dimensions, slot);
		}

		template <BufferResourceType Type = Type>
		static std::shared_ptr<IOutputOnlyRenderTargetView> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == BufferResourceType::OutputOnly)
		{
			return BindablePool::Resolve<IOutputOnlyRenderTargetView>(gfx, dimensions);
		}
	protected:
		std::conditional_t<Type == BufferResourceType::ShaderResource, uint16_t, utl::empty_t> slot;
	};


}