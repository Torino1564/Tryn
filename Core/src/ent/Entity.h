#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		IEntity() = default;
		IEntity(const IEntity& rhs) = delete;
		IEntity(IEntity&& rhs) noexcept {}
		virtual ~IEntity() = default;
	protected:
		gfx::Model model;
	};

	class StaticObject : public IEntity
	{
	public:
		StaticObject(StaticObject&& rhs) noexcept
			:
			IEntity(std::move(rhs))
		{}
		StaticObject(gfx::IGraphics& gfx, std::string path, glm::vec3 scale = { 1.0f,1.0f,1.0f });

	};
}