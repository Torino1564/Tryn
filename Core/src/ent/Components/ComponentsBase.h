#pragma once

namespace tryn::ent
{
#define COMPONENT_TYPES \
		X( Model )\
		X( Camera )\
		X( Behaviour )\
		X( Physics )

	enum class ComponentType
	{
#define X(el) el,
		COMPONENT_TYPES
#undef X
		Count,
		Unknown
	};

	constexpr ComponentType GetTypeByIndex(int index)
	{
		return static_cast<ComponentType>(index);
	}
}