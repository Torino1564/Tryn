#pragma once
#include "Model.h"
#include <memory>

namespace tryn::ent
{
	class Cube : public Model
	{
	public:
		static std::shared_ptr<gfx::VertexBuffer> GetVertexBuffer()
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
				buffer = std::make_shared<gfx::VertexBuffer>(gfx::VertexLayout(gfx::VertexLayout::VertexElement::Position3D));
				for (auto& vertex : verticesBuf)
				{
					buffer->EmplaceBack(vertex);
				}
			}

			return buffer;
		}
		static std::shared_ptr<std::vector<int>> GetCubeIndices()
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
			return indices;

		}

	private:
		static inline std::shared_ptr<std::vector<glm::vec3>> vertices;
		static inline std::shared_ptr<std::vector<int>> indices;
		static inline std::shared_ptr<gfx::VertexBuffer> buffer;
		static inline bool vertInitialized;
		static inline bool indInitialized;
	};
}