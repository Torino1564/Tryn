#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <sstream>

namespace tryn::gfx
{
	class IPrimitiveTopology : public IBindable
	{
	public:
		static std::shared_ptr<IPrimitiveTopology> Resolve(const IGraphics& gfx);

		static std::string GenerateID(const IGraphics& gfx);
	};
}