#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	class IPixelShader : public IBindable
	{
	public:
		static std::shared_ptr<IPixelShader> Resolve(IGraphics& gfx, std::string path)
		{
			return BindablePool::Resolve<IPixelShader>(gfx, path);
		}
		static const std::string GenerateID(IGraphics& gfx, std::string path)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#PixelShader#";
			UID += path;

			return UID;
		}
	};
}