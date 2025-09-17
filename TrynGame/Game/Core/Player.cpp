#include "Player.h"
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/gfx/Model/Model.h>

#include "Core/src/gfx/Render/Techniques/EntityIDTechnique.h"
#include "Core/src/gfx/Render/Techniques/ForwardPhong.h"

using namespace tryn;

Player::Player(ecs::ECS& ecs, const std::string& name, const std::string& modelPath, gfx::IGraphics& gfx)
	:
	Entity(CreateNew<ecs::PositionComponent,
		ecs::ActiveComponent,
		ecs::RotationComponent,
		ecs::ScaleComponent,
		ecs::VelocityComponent,
		ecs::TransformComponent,
		ecs::ModelComponent>(ecs, name)), name(name)
{
	auto& [active] = GetComponent<ecs::ActiveComponent>();
	auto& [position] = GetComponent<ecs::PositionComponent>();
	auto& [yaw, pitch, roll] = GetComponent<ecs::RotationComponent>();
	auto& [scale] = GetComponent<ecs::ScaleComponent>();
	auto& [velocity] = GetComponent<ecs::VelocityComponent>();
	auto& [pModel] = GetComponent<ecs::ModelComponent>();

	active = true;

	position = glm::vec3(0.0f, 10.0f, 0.0f);

	pitch = 0;
	roll = 0;
	yaw = 0;

	scale = glm::vec3(1.0f);

	velocity = glm::vec3(0.0f);

	pModel = std::make_unique<gfx::Model>(gfx, modelPath, std::array{ ZT_TYPE_UUID(gfx::ForwardPhong)});
}
