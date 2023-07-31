#include "Entity.h"
#include <Core/src/gfx/Model/StaticMeshPool.h>

namespace tryn::ent
{
	StaticObject::StaticObject(gfx::IGraphics& gfx, std::string path, glm::vec3 scale)
	{
		pMesh = gfx::StaticMeshPool::Resolve(path, scale);
		InitTransformCBuf(gfx);
		pMesh->MakeBindables(gfx);
	}
}