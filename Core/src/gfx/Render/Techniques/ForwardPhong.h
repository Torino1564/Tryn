#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	template <bool Instanced = false, bool Skinned = false>
	class ForwardPhongBase : public TechniqueBase
	{
	public:
		ForwardPhongBase(class Material& material, aiMaterial& aiMaterial, class IGraphics& gfx, const std::string& path);
	};

	using ForwardPhong = ForwardPhongBase<false, false>;
	using ForwardPhongInst = ForwardPhongBase<true, false>;
	using ForwardPhongSkn = ForwardPhongBase<false, true>;
	using ForwardPhongInstSkn = ForwardPhongBase<true, true>;
}