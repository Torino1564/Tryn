#include "Entity.h"

namespace tryn::ent
{
	void IEntity::Draw(gfx::IGraphics& gfx)
	{
		for (auto& bindable : bindables)
		{
			bindable->Bind();
		}

		gfx.DrawIndexed((int)model->GetIndices()->size());
	}
}