#pragma once
#include <string>
#include <Core/src/ecs/Entity.h>


using namespace tryn;

class Player : public ecs::Entity
{
public:
	Player(std::string name, const std::string& modelPath, tryn::gfx::IGraphics& gfx);
private:

	std::string name;
};