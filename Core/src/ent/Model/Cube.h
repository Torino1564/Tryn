#pragma once
#include "Model.h"

namespace tryn::ent
{
	class Cube : public Model
	{
	public:
		static Cube& GetInstance()
		{
			static Cube instance;
			return instance;
		}
	private:
		Cube()
			: Model(GetCubeVertices(), GetCubeIndices())
		{

		}
		static std::vector<glm::vec3> GetCubeVertices()
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
			return vertices;
		}
		static std::vector<int> GetCubeIndices()
		{
			static const std::vector<int> indices =
			{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			};
			return indices;
		}
	};
}