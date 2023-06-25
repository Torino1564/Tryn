#pragma once
#include <Core/src/spa/Vec3.h>
#include "Bindable.h"
#include <vector>
#include <memory>
#include "Model.h"
#include <Core/src/gfx/Gfx.h>

using namespace tryn::spa;

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity() = 0;
		void Draw( gfx::IGraphics& gfx );

	public:
		Vec3I pos;
		Vec3I vel;
		Vec3I acc;
	protected:
		std::shared_ptr<Model> model;
		std::vector<std::shared_ptr<IBindable>> bindables;
	};
}