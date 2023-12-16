#pragma once
#include "SystemManager.h"
#include <Core/src/ent/Component/ModelComponent.h>

namespace tryn::ent::sys
{
	ZT_DEFINE_SYSTEM(RenderSystem)
	{
	public:
		ZT_SYSTEM_UUID;
		RenderSystem()
		{

		}
		static void Execute()
		{

		}
	};
}