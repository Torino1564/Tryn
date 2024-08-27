#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>

extern "C" __declspec(dllexport) inline auto GetComponents()
{
	return tryn::ecs::ComponentManager::ComponentVector();
}