#pragma once
#include <Core/src/gfx/Gfx.h>

namespace tryn::gfx
{
	class IBindable
	{
	public:
		virtual ~IBindable() = 0;
		virtual void Bind( gfx::IGraphics& gfx ) = 0;
	};
}