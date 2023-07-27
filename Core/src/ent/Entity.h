#pragma once
#include <memory>
#include <Core/src/gfx/Drawable.h>

namespace tryn::ent
{
	class IEntity : public gfx::Drawable
	{
	public:
		IEntity() = default;
		IEntity(const IEntity& rhs) = delete;
		IEntity(IEntity&& rhs) noexcept
			:
			Drawable(std::move(rhs))
		{}
		virtual ~IEntity() = default;
	};

	class StaticObject : public IEntity
	{
	public:
		StaticObject(StaticObject&& rhs) noexcept
			:
			IEntity(std::move(rhs))
		{}
		StaticObject(gfx::IGraphics& gfx, std::shared_ptr<gfx::Mesh> mesh);
		StaticObject(gfx::IGraphics& gfx, std::string path, glm::vec3 scale = { 1.0f,1.0f,1.0f });

	};
}