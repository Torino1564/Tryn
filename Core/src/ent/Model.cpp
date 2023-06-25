#include "Model.h"

namespace tryn::ent
{
	Model::Model(std::vector<float> vertices, std::vector<int> indices, std::optional<std::vector<spa::Vec3F>> normals)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->normals = normals;
	}
}


