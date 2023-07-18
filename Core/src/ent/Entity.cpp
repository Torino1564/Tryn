#include "Entity.h"
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::ent
{
	void IEntity::Draw(gfx::IGraphics& gfx)
	{
		for (auto& bindable : bindables)
		{
			bindable->Bind();
		}
		// TODO FIX
		gfx.DrawIndexed((int)mesh->GetIndexCount());
	}
}