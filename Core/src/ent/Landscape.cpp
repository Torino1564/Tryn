#include "Landscape.h"
#include <Core/src/utl/Assert.h>

#define MATRIX_INDEX(x,y) vertices[ x + (i * rowSize) ]

namespace tryn::ent
{
	Landscape::Landscape(std::string_view texturePath, std::optional<glm::vec3> scale)
	{
		pTexture = gfx::TexturePool::Resolve(texturePath, scale);
		//trynass_msg(pTexture->GetNumChannels() == 2, L"The loaded heightmap is not on a 16bit format!");

		std::vector<glm::vec3> vertices;
		const auto width = pTexture->GetWidth();
		const auto height = pTexture->GetHeight();
		using vec2I = glm::vec<2, int>;
		const auto pData = reinterpret_cast<const std::uint16_t*>(pTexture->Data());

		const auto rowSize = width;

		const auto startX = width / 2;
		const auto startZ = height / 2;

		// Vertex Creation
		vertices.resize(width * height, glm::vec3{.0f,.0f,.0f});
		for (auto i = 0; i < width; i++)
		{
			for (auto j = 0; j < height; j++)
			{
				vertices[i + (rowSize * j)] = glm::vec3(startX + i, pData[i + (rowSize * j)], startZ + j);
			}
		}
		// Index Creation
		std::vector<int> indices;
		indices.resize((width * height) * 6);
		auto index = 0;
		for (auto i = 0; i < width - 1; i++)
		{
			for (auto j = 0; j < height - 1; j++)
			{
				indices[index++] = j + (rowSize * i);
				indices[index++] = j + (rowSize * (i + 1));
				indices[index++] = (j + 1) + (rowSize * (i + 1));
			}
		}

		// Mesh Creation
		trylog.info(L"Finished parsing the heightmap");
	}
}
