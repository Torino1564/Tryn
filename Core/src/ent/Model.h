#pragma once
#include <vector>
#include <Core/src/spa/Vec3.h>

namespace tryn::ent
{
	class Model
	{
	public:
		Model() = delete;
		Model(std::vector<spa::Vec3F>, std::vector<int>, std::vector<spa::Vec3F>);

		std::vector<spa::Vec3F> vertices;
		std::vector<int> indices;
		std::vector<spa::Vec3F> normals;
	};
}