#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <format>
#include <Core/src/gfx/ComparissonMode.h>
#include <Core/src/gfx/Bindables/BufferResourceType.h>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/gfx/RTVDSFwd.h>

namespace tryn::gfx
{
	class IGenericDepthStencil : public IBindable
	{
	public:
		virtual ~IGenericDepthStencil() = default;
		virtual void Clear() const = 0;
		virtual BufferResourceType GetType() const = 0;
	};

	template <BufferResourceType Type>
	class IDepthStencil : public IGenericDepthStencil
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less) requires (Type == BufferResourceType::OutputOnly);
		
		static std::string GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, uint16_t slot, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::ShaderResource);

		static std::shared_ptr<IOutputOnlyDepthStencil> Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::OutputOnly);
		
		static std::shared_ptr<IShaderResourceDepthStencil> Resolve(const IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::ShaderResource);

		BufferResourceType GetType() const override
		{
			return Type;
		}
	protected:
		std::conditional_t<Type == BufferResourceType::ShaderResource, uint16_t, utl::empty_t> slot;
	};
}
