#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <format>
#include <Core/src/gfx/ComparissonMode.h>
#include <Core/src/gfx/Bindables/BufferResourceType.h>
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/utl/EmptyType.h>

namespace tryn::gfx
{
	class IGenericDepthStencil : public IBindable
	{
	public:
		virtual ~IGenericDepthStencil() = default;
		virtual void Clear() const = 0;
	};

	template <BufferResourceType Type>
	class IDepthStencil : public IGenericDepthStencil
	{
	public:
		template <BufferResourceType Type = Type>
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::OutputOnly)
		{
			static uint16_t depthStencil = 0u;

			auto out = std::format("{}#DS#W:{}H:{}#Mode:{}#{}",
				IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
				dimensions.width, dimensions.height, (int)mode, depthStencil++
				);

			return out;
		}
		template <BufferResourceType Type = Type>
		static std::string GenerateID(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::ShaderResource)
		{
			static uint16_t depthStencil = 0u;

			auto out = std::format("{}#DS#W:{}H:{}#Slot:{}#Mode:{}#{}",
				IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
				dimensions.width, dimensions.height, slot, (int)mode, depthStencil++
			);

			return out;
		}
		template<BufferResourceType Type = Type>
		static std::shared_ptr<IOutputOnlyDepthStencil> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::OutputOnly)
		{
			return BindablePool::Resolve<IOutputOnlyDepthStencil>(gfx, dimensions, mode);
		}
		template<BufferResourceType Type = Type>
		static std::shared_ptr<IShaderResourceDepthStencil> Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::ShaderResource)
		{
			return BindablePool::Resolve<IShaderResourceDepthStencil>(gfx, dimensions, slot, mode);
		}

	protected:
		std::conditional_t<Type == BufferResourceType::ShaderResource, uint16_t, utl::empty_t> slot;
	};
}