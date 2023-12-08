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
	class ModelComponent : public Component
	{
		friend class IEntity;
	public:
		ModelComponent(gfx::IGraphics& gfx, int entityID, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		ModelComponent();
		void OnUpdate(double dt = 0) override;
		static constexpr ComponentType GetCUID();
		void Controls() override;
		struct SubresourceData
		{
			glm::mat4 transform = {};
			std::unique_ptr<gfx::Model> model;
			bool active = false;
		};
		static void Execute(std::span<SubresourceData> data);
	private:
		struct Settings
		{
			glm::vec3 angles = {};
			glm::vec3 position = {};
			glm::vec3 scale = { 1.0f,1.0f,1.0f };
		};
		Settings settings;
	};
}