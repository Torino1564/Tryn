#pragma once
#include <Core/src/gfx/GraphicAPI.h>

namespace tryn::gfx
{
	class IContext
	{
	public:
		virtual ~IContext() = default;
		constexpr virtual GraphicAPI GetApi() const = 0;
	};
}