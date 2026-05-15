#include "Shape.h"
#include <utility>
#include <vector>
#include <Core/third/glm/fwd.hpp>

namespace tryn::gfx
{
	size_t Shape3D::NumVertices() const
	{
		return vertices.size();
	}
	const std::vector<glm::vec3>& Shape3D::GenerateNormals()
	{
		// Generate Normals
		normals.resize(NumVertices(), {});

		for (auto i = 0; i < indices.size(); i += 3)
		{
			const auto& v0 = vertices[indices[i]];
			const auto& v1 = vertices[indices[i + 1]];
			const auto& v2 = vertices[indices[i + 2]];

			const auto normal = glm::triangleNormal(v0, v1, v2);

			normals[indices[i]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}

		for (auto& normal : normals)
		{
			normal = glm::normalize(normal);
		}

		return normals;
	}
	Shape3D::Shape3D()
	{
		GenerateNormals();
		GenerateTangentsAndBitangents();
	}
	const std::vector<glm::vec2>& Shape3D::TexCoords() const
	{
		return texcoords;
	}
	const std::vector<glm::vec3>& Shape3D::Vertices() const
	{
		return vertices;
	}
	const std::vector<glm::vec3>& Shape3D::Normals() const
	{
		return normals;
	}
	const std::vector<glm::vec3>& Shape3D::Tangents() const
	{
		return tangents;
	}
	const std::vector<glm::vec3>& Shape3D::Bitangents() const
	{
		return bitangents;
	}
	const std::vector<uint32_t>& Shape3D::Indices() const
	{
		return indices;
	}
	inline std::pair<std::vector<glm::vec3>&, std::vector<glm::vec3>&> Shape3D::GenerateTangentsAndBitangents()
	{
		// Generate Tangents and Bitangents
		return { tangents, bitangents };
	}
}