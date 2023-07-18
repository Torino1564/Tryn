#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/BindablePool.h>
#include <sstream>

namespace tryn::gfx
{
	class IPrimitiveTopology : public IBindable
	{
	public:
		static std::shared_ptr<IPrimitiveTopology> Resolve(IGraphics& gfx)
		{
			return BindablePool::Resolve<IPrimitiveTopology>(gfx);
		}
		static std::string GenerateID( IGraphics& gfx )
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#PrimitiveTopology#TriangleList";
			return ss.str();
		}
	};
}