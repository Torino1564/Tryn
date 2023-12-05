#pragma once
#include <memory>
#include <Core/src/ent/Components/Component.h>
#include <Core/third/glm/mat4x4.hpp>

namespace tryn::gfx
{
	class Model;
	class IGraphics;
}

namespace tryn::ent
{
	class IEntity;
	class ModelComponent : public Component
	{
		friend class IEntity;
	public:
		ModelComponent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		void OnUpdate(double dt = 0) override;
		static constexpr ComponentType GetCUID();
		void Controls() override;
	private:
		struct Settings
		{
			glm::vec3 angles = {};
			glm::vec3 position = {};
			glm::vec3 scale = { 1.0f,1.0f,1.0f };
		};
		Settings settings;
		std::unique_ptr<gfx::Model> model;
		glm::mat4 transform = {};
	};
}