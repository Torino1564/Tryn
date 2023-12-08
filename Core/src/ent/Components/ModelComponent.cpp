#include "ModelComponent.h"
#include <Core/src/gfx/Model/Model.h>
#include <Core/src/gfx/RenderQueue/TechniqueProbe.h>
#include "ComponentManager.h"

namespace tryn::ent
{

	ModelComponent::ModelComponent(gfx::IGraphics& gfx, int entityID, std::string_view path, glm::vec3 scale)
	{
		auto& srd = ComponentManager::Get().AddComponent<ModelComponent>(entityID, *this);
		pSRD = &srd;
		srd.model = std::make_unique<gfx::Model>(gfx, path, scale);
	}
	ModelComponent::ModelComponent()
	{

	}
	void ModelComponent::OnUpdate(double dt)
	{
		glm::mat4 modelTransform(1.0f);
		modelTransform = glm::scale(modelTransform, settings.scale);
		modelTransform = glm::translate(modelTransform, settings.position);
		auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		auto transform = modelTransform * rotation;

		auto& srd = *reinterpret_cast<SubresourceData*>(pSRD);
		srd.transform = transform;
		srd.active = true;
	}

	constexpr ComponentType ModelComponent::GetCUID()
	{
		return ComponentType::Model;
	}
	void ModelComponent::Controls()
	{

	}
	void ModelComponent::Execute(std::span<SubresourceData> data)
	{
		for (auto& element : data)
		{
			if (element.active)
			{
				element.model->Submit(element.transform);
				element.active = false;
			}
		}
	}
}

