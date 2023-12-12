#pragma once
#include "Component.h"

namespace tryn::ent
{
	class BehaviourComponent : public Component
	{
	public:
		static constexpr ComponentType GetCUID()
		{
			return ComponentType::Behaviour;
		}

	};
}