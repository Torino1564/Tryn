#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity() = default;
		void Draw();
	protected:
		std::string name;
		std::unique_ptr<gfx::Model> model;
	};

	class BasicEntity : public IEntity
	{
	public:
		BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
	};
}