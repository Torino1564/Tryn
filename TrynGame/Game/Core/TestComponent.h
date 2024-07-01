#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tgame::cmp
{
	ZT_DEFINE_COMPONENT(TestComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(int, test, 69);
		);
	};
}