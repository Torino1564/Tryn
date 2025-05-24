#pragma once
#include "Bindable.h"
#include <concepts>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/gfx/Bindables/BufferResourceType.h>
#include <Core/src/spa/Dimensions.h>
#include <memory>
#include <Core/src/gfx/RTVDSFwd.h>

namespace tryn::gfx
{
	class IGenericRenderTargetView : public IBindable
	{
	public:
		~IGenericRenderTargetView() override = default;
		virtual void BindAsRTV(IGenericDepthStencil* pDSV = nullptr);
		virtual void Clear() const = 0;
	protected:
		spa::DimensionsI dimensions = {};
	};

	template <BufferResourceType Type = BufferResourceType::OutputOnly>
	class IRenderTargetView : public IGenericRenderTargetView
	{
	public:
		~IRenderTargetView() override = default;

		static std::string GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot, RenderTargetFormat format = RenderTargetFormat::B8G8R8A8_UNORM)
			requires (Type == BufferResourceType::ShaderResource);

		static std::string GenerateID(const IGraphics& gfx, const spa::DimensionsI dimensions, RenderTargetFormat format = RenderTargetFormat::B8G8R8A8_UNORM)
			requires (Type == BufferResourceType::OutputOnly);

		static std::shared_ptr<IShaderResourceRenderTargetView> Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot, RenderTargetFormat format = RenderTargetFormat::B8G8R8A8_UNORM)
			requires (Type == BufferResourceType::ShaderResource);

		static std::shared_ptr<IOutputOnlyRenderTargetView> Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, RenderTargetFormat format = RenderTargetFormat::B8G8R8A8_UNORM)
			requires (Type == BufferResourceType::OutputOnly);
		
	protected:
		BufferResourceType type = Type;
		RenderTargetFormat format = RenderTargetFormat::B8G8R8A8_UNORM;
		std::conditional_t<Type == BufferResourceType::ShaderResource, uint16_t, utl::empty_t> slot;
	};
}
