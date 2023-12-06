#pragma once

namespace tryn::ent
{
#define COMPONENT_TYPES \
		X( Model )\
		X( Camera )\
		X( Physics )\
		X( Count )\
		X( Other )\
		X( Invalid )

	enum class ComponentType
	{
#define X(el) el,
		COMPONENT_TYPES
#undef X
	};
}