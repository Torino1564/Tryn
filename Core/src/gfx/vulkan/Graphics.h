#pragma once
#include <Core/src/gfx/IGraphics.h>
#pragma warning (push)
#pragma warning (disable : 26451 26433)
//#include <vulkan/vulkan.hpp>
#pragma warning (pop)

namespace tryn::gfx::vlk
{
	class Graphics : public IGraphics
	{
		Graphics();
	};
}