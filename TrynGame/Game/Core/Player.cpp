#include "Player.h"
#include <TrynGame/App/GameApp.h>

//Components
#include <Core/src/ecs/cmp/PositionComponent.h>
#include <Core/src/ecs/cmp/RotationComponent.h>
#include <Core/src/ecs/cmp/ScaleComponent.h>
#include <Core/src/ecs/cmp/ModelComponent.h>
#include <Core/src/ecs/cmp/VelocityComponent.h>
#include <Core/src/ecs/cmp/TransformComponent.h>
#include <Core/src/ecs/cmp/ActiveComponent.h>

using namespace tryn;

Player::Player(std::string name, const std::string& modelPath, gfx::IGraphics& gfx)
	:
	Entity(std::move(ecs::Entity::CreateNew<ecs::cmp::PositionComponent,
		ecs::cmp::ActiveComponent,
		ecs::cmp::RotationComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::VelocityComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent>(std::move(name))))
{
	auto& activeCmp = GetComponent<ecs::cmp::ActiveComponent>();
	auto& posCmp = GetComponent<ecs::cmp::PositionComponent>();
	auto& rotCmp = GetComponent<ecs::cmp::RotationComponent>();
	auto& scaleCmp = GetComponent<ecs::cmp::ScaleComponent>();
	auto& velCmp = GetComponent<ecs::cmp::VelocityComponent>();
	auto& modelCmp = GetComponent<ecs::cmp::ModelComponent>();

	activeCmp.active = true;

	posCmp.position = glm::vec3(0.0f, 10.0f, 0.0f);

	rotCmp.pitch = 0;
	rotCmp.roll = 0;
	rotCmp.yaw = 0;

	scaleCmp.scale = glm::vec3(1.0f);

	velCmp.velocity = glm::vec3(0.0f);

	modelCmp.pModel = gfx::Model::Make<gfx::Flat>(gfx, modelPath);
}
