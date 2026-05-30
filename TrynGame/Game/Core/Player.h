#pragma once
#include <string>
#include "Core/src/ecs/Entity.h"


using namespace tryn;

class Player : public ecs::Entity
{
public:
	Player(ecs::ECS& ecs, const std::string& name, const std::string& modelPath, gfx::IGraphics& gfx);
private:

	std::string name;
};