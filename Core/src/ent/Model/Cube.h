#pragma once
#include "Model.h"
#include <memory>

namespace tryn::ent
{
	class Cube : public Model
	{
	public:
		static std::shared_ptr<std::vector<glm::vec3>> GetCubeVertices()
		{
			static const std::vector<glm::vec3> verticesBuf =
			{
				{ -1.0f,-1.0f,-1.0f	 },
				{ 1.0f,-1.0f,-1.0f	 },
				{ -1.0f,1.0f,-1.0f	 },
				{ 1.0f,1.0f,-1.0f	 },
				{ -1.0f,-1.0f,1.0f	 },
				{ 1.0f,-1.0f,1.0f	 },
				{ -1.0f,1.0f,1.0f	 },
				{ 1.0f,1.0f,1.0f	 },
			};

			if (!vertInitialized)
			{
				vertices = std::make_shared<std::vector<glm::vec3>>(verticesBuf);
				vertInitialized = true;
			}

			return vertices;
		}
		static std::vector<int> GetCubeIndices()
		{
			static const std::vector<int> indicesBuf =
			{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			};
			
			if (!indInitialized)
			{
				indices = std::make_shared<std::vector<int>>(indicesBuf);
				indInitialized = true;
			}

		}

	private:
		static std::shared_ptr<std::vector<glm::vec3>> vertices;
		static std::shared_ptr<std::vector<int>> indices;
		static bool vertInitialized;
		static bool indInitialized;
	};
}