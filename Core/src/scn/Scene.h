#pragma once
#include <vector>
#include <Core/src/ecs/Entity.h>
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/sys/SystemManager.h>

namespace tryn::scn
{
	class Scene
	{
	private:

		std::vector<std::unique_ptr<ecs::Entity>> entities;
	};
}