#include "ComponentManager.h"
#include <Core/third/imgui/imgui.h>

namespace tryn::ent
{
	ComponentManager::ComponentManager()
	{
		memory = Components_Ty();
		ResizeArrays(10);
	}
}