#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		struct Settings
		{
		public:
			glm::vec3 angles = {};
			glm::vec3 position = {};
			glm::vec3 scale = { 1.0f,1.0f,1.0f };
		};
	public:
		virtual ~IEntity() = default;
		void Submit();
		void SpawnControlWindow();
	public:
		Settings settings;

	protected:
		std::string name;
		std::unique_ptr<gfx::Model> model;
		glm::mat4 transform = {};
	};

	class BasicEntity : public IEntity
	{
	public:

		BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		BasicEntity(const BasicEntity&) = delete;
		BasicEntity& operator=(const BasicEntity&) = delete;

		BasicEntity(BasicEntity&&) = default;
		BasicEntity& operator=(BasicEntity&&) = default;
	};
}