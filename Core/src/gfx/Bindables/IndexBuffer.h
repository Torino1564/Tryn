#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>

namespace tryn::gfx
{
	class IIndexBuffer : public IBindable
	{
	public:
		static std::string GenerateID(IGraphics& gfx, std::string tag , std::shared_ptr<const std::vector<int>> indices)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#IndexBuffer#";
			UID += std::to_string(indices->size());
			UID += '#';
			UID += tag;
			return UID;
		}
	protected:
		std::string tag;
	};
}