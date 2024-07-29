#include "VertexShader.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::shared_ptr<IVertexShader> IVertexShader::Resolve(const IGraphics& gfx, const std::string& path)
	{
		return BindablePool::Resolve<IVertexShader>(gfx, path);
	}

	std::string IVertexShader::GenerateID(const IGraphics& gfx, const std::string& path)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#VertexShader#";
		UID += path;

		return UID;
	}

	std::string_view IVertexShader::GetPath() const
	{
		return path;
	}
}
