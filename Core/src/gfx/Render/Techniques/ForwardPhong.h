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

		// The material vector has to contain the PBR material on slot 0
		ForwardPhong(const std::vector<std::shared_ptr<Material>>& materials, const IGraphics& gfx, bool instanced = false, bool skinned = false);
	};
}