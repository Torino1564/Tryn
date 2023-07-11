#pragma once
#include <Core/src/gfx/Gfx.h>

namespace tryn::gfx
{
	class IBindable
	{
	public:
		virtual ~IBindable() {}
		virtual void Bind( ) = 0;
		GraphicAPI GetAPI() const
		{
			return type;
		}
	protected:
		GraphicAPI type = GraphicAPI::Unknown;
	};
}