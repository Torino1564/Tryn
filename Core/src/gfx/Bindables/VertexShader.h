#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <string>

namespace tryn::gfx
{
	class IVertexShader : public IBindable
	{
	public:
		static const std::string GenerateID(IGraphics& gfx, std::string path)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#VertexShader#";
			UID += path;
			
			return UID;
		}
	protected:
		std::string path;
	};
}