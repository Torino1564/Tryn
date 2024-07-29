#pragma once
#include <Core/src/utl/String.h>
#include <Core/src/gfx/GraphicAPI.h>

namespace tryn::gfx
{
	class IGraphics;
	class IContext;
	class IBindable
	{
	public:
		virtual ~IBindable();
		virtual void Bind(IContext&);
		virtual void Bind() = 0;
		virtual void Accept(class TechniqueProbe&);
		GraphicAPI GetAPI() const;

	protected:
		GraphicAPI type = GraphicAPI::Unknown;
	};
}