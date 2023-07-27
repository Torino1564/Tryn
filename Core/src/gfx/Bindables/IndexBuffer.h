#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	class IIndexBuffer : public IBindable
	{
	public:
		static std::shared_ptr<IIndexBuffer> Resolve(IGraphics& gfx, std::shared_ptr<const std::vector<int>> indices, std::string tag = "?")
		{
			return BindablePool::Resolve<IIndexBuffer>(gfx, indices, tag);
		}
		static std::string GenerateID(IGraphics& gfx, std::shared_ptr<const std::vector<int>> indices, std::string tag = "?")
		{
			if (tag == "?") return tag;
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#IndexBuffer#";
			UID += std::to_string(indices->size());
			UID += '#';
			UID += tag;
			return UID;
		}
	protected:
		std::string tag;
		std::shared_ptr<const std::vector<int>> indices;
		int count = 0;
	};
}