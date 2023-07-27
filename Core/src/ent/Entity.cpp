#include "Entity.h"
#include <Core/src/gfx/Bindables/ConstantBuffer.h>
#include <Core/src/gfx/Mesh/StaticMeshPool.h>

namespace tryn::ent
{
	StaticObject::StaticObject(gfx::IGraphics& gfx, std::shared_ptr<gfx::Mesh> mesh)
	{
		trynass_msg(mesh->IsStatic(), L"Cant create a static object with a non static mesh!");
		this->pMesh = mesh;
		InitTransformCBuf(gfx);
		pMesh->MakeBindables(gfx);
	}
	StaticObject::StaticObject(gfx::IGraphics& gfx, std::string path, glm::vec3 scale)
	{
		pMesh = gfx::StaticMeshPool::Resolve(path, scale);
		InitTransformCBuf(gfx);
		pMesh->MakeBindables(gfx);
	}
}