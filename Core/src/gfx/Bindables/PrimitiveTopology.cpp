#include "PrimitiveTopology.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::shared_ptr<IPrimitiveTopology> IPrimitiveTopology::Resolve(const IGraphics& gfx)
	{
		return BindablePool::Resolve<IPrimitiveTopology>(gfx);
	}

	std::string IPrimitiveTopology::GenerateID(const IGraphics& gfx)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::stringstream ss;
		ss << typeStr << "#PrimitiveTopology#TriangleList";
		return ss.str();
	}
}
