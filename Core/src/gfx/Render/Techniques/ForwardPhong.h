#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	class IGraphics;
	class ForwardPhong;
	class ForwardPhong : public Technique<ForwardPhong>
	{
	public:
		ForwardPhong(const std::string& name);
		ForwardPhong(Material& material, const aiMaterial& aiMat, const IGraphics& gfx, const std::string& rootPath, bool instanced = false, bool skinned = false);
	};
}