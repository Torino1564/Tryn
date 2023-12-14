#pragma once
#include "Component.h"

namespace tryn::ent
{
	class PhysicsComponent
	{
		friend class IEntity;
	public:
		static constexpr ComponentType GetUID();
		PhysicsComponent() = default;
		struct SubresourceData
		{
			bool active = false;
		};
	};
}