#pragma once
#include "Bindable.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	class IRasterizer : public IBindable
	{
	public:
		static std::shared_ptr<IRasterizer> Resolve(IGraphics& gfx, bool twoSided = true)
		{
			return BindablePool::Resolve<IRasterizer>(gfx, twoSided);
		}
		static std::string GenerateID(IGraphics& gfx, bool twoSided = true)
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
				UID += "2";
			}
			return UID;
		}

	protected:
		bool twoSided;
	};
}