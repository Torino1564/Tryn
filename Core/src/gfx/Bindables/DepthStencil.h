#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/ComparissonMode.h>
#include <Core/src/spa/Dimensions.h>

namespace tryn::gfx
{
	class IDepthStencil : public IBindable
	{
	public:
		virtual void Clear() const = 0;

		static std::string GenerateID(const IGraphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot = std::nullopt, ComparissonMode mode = ComparissonMode::Less);

		static std::shared_ptr<IDepthStencil> Resolve(const IGraphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot = std::nullopt, ComparissonMode mode = ComparissonMode::Less);

		bool IsShaderResource() const;

	protected:
		bool shaderResource = true;
		uint16_t slot = 0;
	};
}
