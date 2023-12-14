#pragma once
#include <memory>
#include <Core/src/ent/Components/Component.h>
#include <Core/third/glm/mat4x4.hpp>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::ent
{
	class IEntity;

	class ModelComponent
	{
	public:
		ModelComponent(gfx::IGraphics& gfx, int entityID, std::string_view path, glm::vec3 scale);
		static constexpr ComponentType GetUID();
		struct SubresourceData
		{
			bool active = false;
			glm::mat4 transform = {};
			std::unique_ptr<gfx::Model> model;
		};
	};
}