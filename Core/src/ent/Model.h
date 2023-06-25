#pragma once
#include <vector>
#include <optional>
#include <Core/src/spa/Vec3.h>

namespace tryn::ent
{
	class Model
	{
	public:
		Model() = delete;
		Model(std::vector<float>, std::vector<int>, std::optional<std::vector<spa::Vec3F>>);

		std::vector<float> vertices;
		std::vector<int> indices;
		std::optional<std::vector<spa::Vec3F>> normals;
	};
}