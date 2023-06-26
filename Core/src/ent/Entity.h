#pragma once
#include <Core/src/spa/Vec3.h>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <memory>
#include "Model.h"
#include <Core/src/gfx/Gfx.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity() = 0;
		void Draw( gfx::IGraphics& gfx );
	protected:
		std::shared_ptr<Model> model;
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	};
}