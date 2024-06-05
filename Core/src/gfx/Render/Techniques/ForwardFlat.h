#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	class ForwardFlat : public Technique
	{
	public:
		ForwardFlat(class Material& material, aiMaterial& aiMat, class IGraphics& gfx, const std::string& path, bool instanced, bool skinned);
	};
}