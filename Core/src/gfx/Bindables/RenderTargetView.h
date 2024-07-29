#pragma once
#include "Bindable.h"
#include <concepts>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/gfx/Bindables/BufferResourceType.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	class IGenericRenderTargetView : public IBindable
	{
	public:
		~IGenericRenderTargetView() override = default;
		virtual void BindAsRTV(IGenericDepthStencil* pDSV);
		virtual void Clear() const = 0;
	protected:
		spa::DimensionsI dimensions = {};
	};

	template <BufferResourceType Type = BufferResourceType::OutputOnly>
	class IRenderTargetView : public IGenericRenderTargetView
	{
	public:
		~IRenderTargetView() override = default;

		static std::string GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource);

		static std::string GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == BufferResourceType::OutputOnly);

		static std::shared_ptr<IShaderResourceRenderTargetView> Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource);

		static std::shared_ptr<IOutputOnlyRenderTargetView> Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == BufferResourceType::OutputOnly);
		
	protected:
		std::conditional_t<Type == BufferResourceType::ShaderResource, uint16_t, utl::empty_t> slot;
	};
}
