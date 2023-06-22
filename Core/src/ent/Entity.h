#pragma once
#include <Core/src/spa/Vec3.h>

using namespace tryn::spa;

namespace tryn::ent
{
	struct Model
	{

	};
	struct Material
	{

	};

	class Entity
	{
	public:
		Vec3I pos;
		Vec3I vel;
	private:
		Model model;
		Material material;
	};
}