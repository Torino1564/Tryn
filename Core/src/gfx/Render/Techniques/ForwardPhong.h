#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	class ForwardPhong : public Technique
	{
	public:
		ForwardPhong(class Material& material, aiMaterial& aiMaterial, class IGraphics& gfx, const std::string& path, bool instanced, bool skinned);
	};
}