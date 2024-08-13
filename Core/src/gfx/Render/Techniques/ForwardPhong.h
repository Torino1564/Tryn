#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	class IGraphics;

	template <bool Instanced = false, bool Skinned = false>
	class ForwardPhongBase : public Technique<ForwardPhongBase, "ForwardPhongBase", Instanced, Skinned>
	{
	public:
		ForwardPhongBase(const std::string& name);
		ForwardPhongBase(class Material& material, const aiMaterial& aiMaterial, const IGraphics& gfx, const std::string& path);
	};

	using ForwardPhong = ForwardPhongBase<false, false>;
	using ForwardPhongInst = ForwardPhongBase<true, false>;
	using ForwardPhongSkn = ForwardPhongBase<false, true>;
	using ForwardPhongInstSkn = ForwardPhongBase<true, true>;
}