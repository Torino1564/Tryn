#pragma once

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::ecs
{
	class Entity;
	class ECS;

	void AddEntityIDJITBuffer(Entity& entity, ECS& ecs);
}