#pragma once
#include <Core/src/gfx/Gfx.h>

namespace tryn::gfx
{
	class IBindable
	{
	public:
		virtual ~IBindable() {}
		virtual void Bind( ) = 0;
	};
}