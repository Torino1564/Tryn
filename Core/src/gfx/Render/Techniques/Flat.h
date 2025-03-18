#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	class IGraphics;

	class Flat : public Technique<Flat>
	{
	public:
		Flat(const std::string& name);
		Flat(Material& material, const aiMaterial& aiMat, const IGraphics& gfx, const std::string& path, bool instanced = false, bool skinned = false);
	};
}