#pragma once
#include <memory>
#include <Core/src/gfx/Bindables/Bindable.h>

namespace tryn::gfx
{
	class IPixelShader : public IBindable
	{
	public:
		static std::shared_ptr<IPixelShader> Resolve(const IGraphics& gfx, const std::string& path);

		static std::string GenerateID(const IGraphics& gfx, const std::string& path);
	};
}