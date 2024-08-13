#pragma once
#include "Core/third/glm/gtx/normal.hpp"

namespace tryn::gfx
{
	class Shape3D
	{
	public:
		Shape3D()
		{
			GenerateNormals();
			GenerateTangentsAndBitangents();
		}
		auto& TexCoords() const
		{
			return texcoords;
		}
		auto& Vertices() const
		{
			return vertices;
		}
		auto& Normals() const
		{
			return normals;
		}
		auto& Tangents() const
		{
			return tangents;
		}
		auto& Bitangents() const
		{
			return bitangents;
		}
		auto& Indices() const
		{
			return indices;
		}
		auto NumVertices() const
		{
			return vertices.size();
		}
		auto& GenerateNormals()
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
		std::pair<std::vector<glm::vec3>&, std::vector<glm::vec3>&> GenerateTangentsAndBitangents()
		{
			// Generate Tangents and Bitangents
			return {tangents, bitangents};
		}
	private:
		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<glm::vec2> texcoords;
	};
}
