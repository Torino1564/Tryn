#pragma once
#include <memory>

namespace tryn::gfx
{
	class Model;
}

namespace tryn::ecs
{
	struct ModelComponent
	{
		~ModelComponent();
		ModelComponent() = default;
		ModelComponent(const ModelComponent& rhs) = delete;
		ModelComponent(ModelComponent&& rhs) noexcept;
		ModelComponent operator=(const ModelComponent& rhs) = delete;
		ModelComponent operator=(ModelComponent&& rhs) const noexcept;

		std::unique_ptr<gfx::Model> pModel;
	};
}