#pragma once
#include <Core/src/gfx/Gfx.h>
#include <Core/src/utl/String.h>

namespace tryn::gfx
{
	class IContext;
	class IBindable
	{
	public:
		virtual ~IBindable() {}
		virtual void Bind(IContext&) { Bind(); };
		virtual void Bind() = 0;
		GraphicAPI GetAPI() const
		{
			return type;
		}
	protected:
		GraphicAPI type = GraphicAPI::Unknown;
	};
}