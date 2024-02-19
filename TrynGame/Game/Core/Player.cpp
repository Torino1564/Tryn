#include "Player.h"
#include <TrynGame/App/GameApp.h>

//Components
#include <Core/src/ecs/cmp/PositionComponent.h>
#include <Core/src/ecs/cmp/RotationComponent.h>
#include <Core/src/ecs/cmp/ScaleComponent.h>
#include <Core/src/ecs/cmp/ModelComponent.h>
#include <Core/src/ecs/cmp/VelocityComponent.h>

using namespace tryn;

Player::Player(std::string name, const std::string& modelPath, gfx::IGraphics& gfx)
	:
	Entity(std::move(ecs::Entity::CreateNew<ecs::cmp::PositionComponent,
		ecs::cmp::RotationComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::VelocityComponent,
		ecs::cmp::ModelComponent>(std::move(name))))
{
	auto& posCmp = GetComponent<ecs::cmp::PositionComponent>();
	auto& rotCmp = GetComponent<ecs::cmp::RotationComponent>();
	auto& scaleCmp = GetComponent<ecs::cmp::ScaleComponent>();
	auto& velCmp = GetComponent<ecs::cmp::VelocityComponent>();
	auto& modelCmp = GetComponent<ecs::cmp::ModelComponent>();

	posCmp.position = glm::vec3(0.0f, 0.0f, 0.0f);

	rotCmp.pitch = 0;
	rotCmp.roll = 0;
	rotCmp.yaw = 0;

	scaleCmp.scale = glm::vec3(1.0f);

	velCmp.velocity = glm::vec3(0.0f);

	modelCmp.pModel = std::make_unique<gfx::Model>(gfx, modelPath);
}
