#include "PixelShader.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::shared_ptr<IPixelShader> IPixelShader::Resolve(const IGraphics& gfx, const std::string& path)
	{
		return BindablePool::Resolve<IPixelShader>(gfx, path);
	}

	std::string IPixelShader::GenerateID(const IGraphics& gfx, const std::string& path)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#PixelShader#";
		UID += path;

		return UID;
	}
}
