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
		// TODO FIX THIS SHIT
		constantBuffers[0]->Bind();
		gfx.DrawIndexed((int)mesh->GetIndexCount());
	}
}