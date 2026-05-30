#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <tuple>

namespace tryn::gfx
{
	class Shape3D
	{
	public:
		Shape3D();
		const std::vector<glm::vec2>& TexCoords() const;
		const std::vector<glm::vec3>& Vertices() const;
		const std::vector<glm::vec3>& Normals() const;
		const std::vector<glm::vec3>& Tangents() const;
		const std::vector<glm::vec3>& Bitangents() const;
		const std::vector<uint32_t>& Indices() const;
		size_t NumVertices() const;
		const std::vector<glm::vec3>& GenerateNormals();
		std::pair<std::vector<glm::vec3>&, std::vector<glm::vec3>&> GenerateTangentsAndBitangents();

	private:
		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<glm::vec2> texcoords;
	};
}
