#pragma once
#include <Core/src/spa/Vec3.h>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <memory>
#include <Core/src/gfx/Mesh/Mesh.h>
#include <Core/src/gfx/Gfx.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity() {}
		void Draw( gfx::IGraphics& gfx );

		std::shared_ptr<gfx::IConstantBuffer> pConstantBuffer;
		std::shared_ptr<gfx::Mesh> mesh;
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	};

	class Entity : public IEntity
	{

	};
}