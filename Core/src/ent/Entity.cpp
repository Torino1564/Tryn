#include "Entity.h"
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::ent
{
	void IEntity::Draw(gfx::IGraphics& gfx)
	{
		for (auto& bindable : otherBindables)
		{
			bindable->Bind();
		}
		pVertexBuffer->Bind();
		pIndexBuffer->Bind();
		pConstantBuffer->Bind();
		gfx.DrawIndexed((int)mesh->GetIndexCount());
	}
}