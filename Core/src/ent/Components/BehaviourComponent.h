#pragma once
#include "Component.h"

namespace tryn::ent
{
	class BehaviourComponent
	{
	public:
		static constexpr ComponentType GetUID()
		{
			return ComponentType::Behaviour;
		}
		struct SubresourceData
		{
			bool active = false;
		};
	};
}