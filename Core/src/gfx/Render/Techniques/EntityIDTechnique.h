#pragma once
#include <Core/src/gfx/Render/Technique.h>

namespace tryn::gfx
{
	class EntityIDTechnique : public Technique<EntityIDTechnique>
	{
	public:
		EntityIDTechnique(const std::string& name);
		// No material is used for this technique. The first parameter is ignored
		EntityIDTechnique(const std::vector<std::shared_ptr<Material>>& materials, const IGraphics& gfx, bool instanced = false, bool skinned = false);
	};
}