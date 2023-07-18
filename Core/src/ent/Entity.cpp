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
		gfx.DrawIndexed((int)mesh->GetIndices()->size());
	}
}