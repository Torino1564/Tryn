#pragma once
#include <Core/src/spa/Vec3.h>
#include <vector>
#include <memory>
#include <Core/src/gfx/Gfx.h>
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/Drawable.h>

namespace tryn::ent
{
	class IEntity : public gfx::Drawable
	{
	public:
		virtual ~IEntity() {}
	};

	class StaticObject : public IEntity
	{
	public:
		StaticObject( gfx::IGraphics& gfx, std::shared_ptr<gfx::Mesh> mesh)
		{
			trynass_msg(mesh->IsStatic(), L"Cant create a static object with a non static mesh!");
			this->pMesh = mesh;
			InitTransformCBuf(gfx);
		}
		StaticObject(std::string path, glm::vec3 scale = { 1.0f,1.0f,1.0f })
		{
			pMesh = gfx::StaticMeshPool::Resolve(path , scale);
		}
		
	};
}