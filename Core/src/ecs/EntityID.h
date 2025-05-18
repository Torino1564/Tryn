#pragma once
#include <cstdint>

namespace tryn::ecs
{
	struct EntityID
	{
		uint32_t ID = 0;
		uint32_t archetype = 0;
	};
}