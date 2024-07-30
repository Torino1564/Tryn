#include "TrynPCH.h"
#include "DepthStencil.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	template <BufferResourceType Type>
	std::string IDepthStencil<Type>::GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode)
		requires (Type == BufferResourceType::OutputOnly)
	{
		static uint16_t depthStencil = 0u;

		auto out = std::format("{}#DS#W:{}H:{}#Mode:{}#{}",
			IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
			dimensions.width, dimensions.height, (int)mode, depthStencil++
			);

		return out;
	}

	template <BufferResourceType Type>
	std::string IDepthStencil<Type>::GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, const uint16_t slot, ComparissonMode mode)
		requires (Type == BufferResourceType::ShaderResource)
	{
		static uint16_t depthStencil = 0u;

			auto out = std::format("{}#DS#W:{}H:{}#Slot:{}#Mode:{}#{}",
				IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())],
				dimensions.width, dimensions.height, slot, (int)mode, depthStencil++
			);

			return out;
	}

	template <BufferResourceType Type>
	std::shared_ptr<IOutputOnlyDepthStencil> IDepthStencil<Type>::Resolve(const IGraphics& gfx,
		const spa::DimensionsI dimensions, ComparissonMode mode) requires (Type == BufferResourceType::OutputOnly)
	{
			return BindablePool::Resolve<IOutputOnlyDepthStencil>(gfx, dimensions, mode);
	}

	template <BufferResourceType Type>
	std::shared_ptr<IShaderResourceDepthStencil> IDepthStencil<Type>::Resolve(const IGraphics& gfx,
		const spa::DimensionsI dimensions, uint16_t slot,
		ComparissonMode mode) requires (Type == BufferResourceType::ShaderResource)
	{
			return BindablePool::Resolve<IShaderResourceDepthStencil>(gfx, dimensions, slot, mode);
	}

	template class IDepthStencil<BufferResourceType::OutputOnly>;
	template class IDepthStencil<BufferResourceType::ShaderResource>;
}
