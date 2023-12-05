#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include "Components/ComponentManager.h"

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity() = default;
		void SpawnControlWindow();
		void Update(double dt = 0);
		ComponentManager componentManager;
	protected:
		std::string name;
		int UID;
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