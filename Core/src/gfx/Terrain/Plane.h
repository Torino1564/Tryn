#pragma once
#include "Core/src/gfx/Model/StaticMesh.h"
#include <Core/src/gfx/Shape.h>

namespace tryn::gfx
{
	class Plane : public Shape3D
	{
	public:
		Plane(unsigned int tesselationLevel, spa::DimensionsI dimensions)
		{
			// Parameters
			this->tesselationLevel = tesselationLevel;
			this->dimensions = dimensions;

			// Shape Creation
			const unsigned long pointsPerSide = std::pow(2, tesselationLevel) + 1;

			vertices.reserve(pointsPerSide * pointsPerSide);
			indices.reserve((pointsPerSide - 1) * (pointsPerSide - 1) * 2 * 3);
			const double xStrideLength = dimensions.height / pointsPerSide;
			const double zStrideLength = dimensions.width / pointsPerSide;

			// vertices
			for (auto x = 0; x < pointsPerSide; x++)
			{
				for (auto z = 0; z < pointsPerSide; z++)
				{
					vertices.emplace_back( x * xStrideLength, 0, z * zStrideLength);
				}
			}

			// indices
			for (auto x = 0; x < pointsPerSide - 1; x++)
			{
				for (auto z = 0; z < pointsPerSide - 1; z++)
				{
					// Quad

					// First triangle
					indices.emplace_back(x + z * pointsPerSide);
					indices.emplace_back(x + 1 + (z + 1)*pointsPerSide );
					indices.emplace_back(x + (z + 1)*pointsPerSide );

					// Second triangle
					indices.emplace_back(x + z * pointsPerSide);
					indices.emplace_back(x + 1 + z*pointsPerSide );
					indices.emplace_back(x + 1 + (z + 1)*pointsPerSide );
				}
			}
		}
	private:
		unsigned int tesselationLevel = 0;
		spa::DimensionsI dimensions;
	};
}
