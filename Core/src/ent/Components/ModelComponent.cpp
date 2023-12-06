#include "ModelComponent.h"
#include <Core/src/gfx/Model/Model.h>
#include <Core/src/gfx/RenderQueue/TechniqueProbe.h>

namespace tryn::ent
{

	ModelComponent::ModelComponent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale)
	{
		model = std::make_unique<gfx::Model>(gfx, path, scale);
	}
	ModelComponent::ModelComponent()
	{
		model = nullptr;
	}
	void ModelComponent::OnUpdate(double dt)
	{
		glm::mat4 modelTransform(1.0f);
		modelTransform = glm::scale(modelTransform, settings.scale);
		modelTransform = glm::translate(modelTransform, settings.position);
		auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		auto transform = modelTransform * rotation;
		model->Submit(transform);
	}

	constexpr ComponentType ModelComponent::GetCUID()
	{
		return ComponentType::Model;
	}
	void ModelComponent::Controls()
	{

	}
}

