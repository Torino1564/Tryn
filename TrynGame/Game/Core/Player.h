#pragma once
#include <string>
#include <TrynGame/Engine/TrynGameEngine.h>

using namespace tryn;

class Player : public ecs::Entity
{
public:
	Player(std::string name, const std::string& modelPath, tryn::gfx::IGraphics& gfx);
private:

	std::string name;
};