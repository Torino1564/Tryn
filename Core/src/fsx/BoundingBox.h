#pragma once
#include "IBoundingBox.h"

namespace tryn::fsx
{
	class BoundingBox : IBoundingBody
	{
	public:
		BoundingBox(glm::vec3 scales) : scales(scales) {}
		bool Intersects(const IBoundingBody& rhs) override;

	private:
		glm::vec3 worldPosition;
		glm::vec3 scales;
	};
}