#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/BindablePool.h>
#include <string>

namespace tryn::gfx
{
	class IVertexShader : public IBindable
	{
	public:
		static std::shared_ptr<IVertexShader> Resolve(IGraphics& gfx, std::string path)
		{
			return BindablePool::Resolve<IVertexShader>(gfx, path);
		}
		static const std::string GenerateID(IGraphics& gfx, std::string path)
		{
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#VertexShader#";
			UID += path;
			
			return UID;
		}
		std::string GetPath() const
		{
			return path;
		}
	protected:
		std::string path;
	};
}