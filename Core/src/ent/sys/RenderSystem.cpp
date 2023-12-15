#include "RenderSystem.h"
#include <Core/src/ent/Component/ComponentManager.h>
#include <Core/src/ent/Component/ModelComponent.h>
#include <Core/src/ent/Component/PositionComponent.h>

void tryn::ent::sys::RenderSystemGo()
{
	auto models = ComponentManager::Get().GetData<cmp::ModelComponent>();
}
