#include "Cube.h"
#include <Core/src/ent/Model/Model.h>
#include <memory>
#include <Core/third/glm/glm.hpp>

namespace tryn::ent
{
	
		static const std::vector<glm::vec3> vertices =
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

		static const std::vector<int> indices = 
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		
}