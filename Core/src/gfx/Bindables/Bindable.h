#pragma once
#include <Core/src/utl/String.h>
#include <Core/src/gfx/GraphicAPI.h>
#include <memory>

namespace tryn::gfx
{
	class Attr_SkipRegister {};
	inline constexpr Attr_SkipRegister attr_SkipRegister;

	class IGraphics;
	class IContext;

	class IBindable
	{
	public:
		virtual ~IBindable();
		virtual void Bind(const IContext&);
		virtual void Bind() = 0;
		virtual void Accept(class TechniqueProbe&);
		GraphicAPI GetAPI() const;

	protected:

		GraphicAPI type = GraphicAPI::Unknown;
	};
}