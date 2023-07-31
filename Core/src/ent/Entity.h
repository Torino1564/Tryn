#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		IEntity() = default;
		virtual ~IEntity() = default;
	protected:
		gfx::Model model;
	};
}