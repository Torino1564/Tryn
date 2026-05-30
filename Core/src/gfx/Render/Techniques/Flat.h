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
		// Only the material at slot 0 is used
		Flat(const std::vector<std::shared_ptr<Material>>& materials, const IGraphics& gfx, bool instanced = false, bool skinned = false);
	};
}