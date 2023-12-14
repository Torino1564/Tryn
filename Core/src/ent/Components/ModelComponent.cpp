#include "ModelComponent.h"
#include <Core/src/gfx/Model/Model.h>
#include <Core/src/gfx/RenderQueue/TechniqueProbe.h>
#include "ComponentManager.h"

namespace tryn::ent
{

	ModelComponent::ModelComponent(gfx::IGraphics& gfx, int entityID, std::string_view path, glm::vec3 scale)
	{
		/*auto& srd = ComponentManager::Get().AddComponent<ModelComponent>(entityID, *this);
		pSRD = &srd;
		srd.model = std::make_unique<gfx::Model>(gfx, path, scale);
		srd.active = false;
		srd.transform = {};*/
	}
	constexpr ComponentType ModelComponent::GetUID()
	{
		return ComponentType::Model;
	}
}

