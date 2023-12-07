#pragma once
#include "Component.h"

namespace tryn::ent
{
	class PhysicsComponent : public Component
	{
		friend class IEntity;
	public:
		static constexpr ComponentType GetUID();
		PhysicsComponent() = default;
	};
}