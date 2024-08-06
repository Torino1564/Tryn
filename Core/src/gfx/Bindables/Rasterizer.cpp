#include "TrynPCH.h"
#include "Rasterizer.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::shared_ptr<IRasterizer> IRasterizer::Resolve(const IGraphics& gfx, bool twoSided)
	{
		return BindablePool::Resolve<IRasterizer>(gfx, twoSided);
	}

	std::string IRasterizer::GenerateID(const IGraphics& gfx, bool twoSided)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#Rasterizer#";
		if (twoSided)
		{
			UID += "2";
		}
		else
		{
			UID += "1";
		}
		return UID;
	}
}
