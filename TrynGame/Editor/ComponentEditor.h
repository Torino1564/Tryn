#pragma once
#include <Core/include/DllApi.h>
#include <Core/src/ecs/cmp/ComponentManager.h>

TRYN_EXPORT inline auto GetComponents()
{
	return tryn::ecs::ComponentManager::ComponentVector();
}