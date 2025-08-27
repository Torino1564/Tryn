
#include "ModelComponent.h"
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ecs
{
	ModelComponent::~ModelComponent() = default;

	ModelComponent::ModelComponent(ModelComponent&& rhs) noexcept
		:
	pModel(std::move(rhs.pModel))
	{

	}

	ModelComponent ModelComponent::operator=(ModelComponent&& rhs) const noexcept
	{
		return ModelComponent(std::move(rhs));
	}
}
