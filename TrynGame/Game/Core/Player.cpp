#include "TrynGameEngine.h"

#include "Player.h"

using namespace tryn;

Player::Player(std::string name, const std::string& modelPath, gfx::IGraphics& gfx)
	:
	Entity(ecs::Entity::CreateNew<ecs::cmp::PositionComponent,
		ecs::cmp::ActiveComponent,
		ecs::cmp::RotationComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::VelocityComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent>(std::move(name)))
{
	auto& [active] = GetComponent<ecs::cmp::ActiveComponent>();
	auto& [position] = GetComponent<ecs::cmp::PositionComponent>();
	auto& [yaw, pitch, roll] = GetComponent<ecs::cmp::RotationComponent>();
	auto& [scale] = GetComponent<ecs::cmp::ScaleComponent>();
	auto& [velocity] = GetComponent<ecs::cmp::VelocityComponent>();
	auto& [pModel] = GetComponent<ecs::cmp::ModelComponent>();

	active = true;

	position = glm::vec3(0.0f, 10.0f, 0.0f);

	pitch = 0;
	roll = 0;
	yaw = 0;

	scale = glm::vec3(1.0f);

	velocity = glm::vec3(0.0f);

	pModel = gfx::Model::Make<gfx::ForwardPhongBase>(gfx, modelPath);
}
