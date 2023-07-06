#include "Cube.h"
#include <Core/src/spa/Vec3.h>
#include <array>

using namespace tryn::spa;

namespace tryn::ent
{
	Cube::Cube(float size)
	{
		if (!isStaticInitialized)
		{
			// create the model
			std::vector<Vec3F> vertices =
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

			std::vector<int> indeces =
			{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			};

			std::vector<spa::Vec3F> normals = {};

			model = std::make_shared<ent::Model>(std::move(vertices), std::move(indeces), std::move(normals));
			
			isStaticInitialized = true;
		}
	}
	const std::shared_ptr<Model> Cube::GetModel() const
	{
		return model;
	}
}


