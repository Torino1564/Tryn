#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>

extern "C" __declspec(dllexport) inline void GetComponents(tryn::ecs::IComponent** outPtr, unsigned int* numElements)
{
	for (auto& pComponent : tryn::ecs::ComponentManager::ComponentMap())
	{
		
	}
}