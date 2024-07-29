#pragma once
#include <Core/src/gfx/Render/Technique.h>

struct aiMaterial;

namespace tryn::gfx
{
	class IGraphics;

	template <bool Instanced = false, bool Skinned = false>
	class FlatBase : public Technique<FlatBase, "FlatBase", Instanced, Skinned>
	{
	public:
		FlatBase(const std::string& name);
		FlatBase(class Material& material, aiMaterial& aiMaterial, const IGraphics& gfx, const std::string& path);
	};

	using Flat =		FlatBase<false, false>;
	using FlatInst =	FlatBase<true, false>;
	using FlatSkn =		FlatBase<false, true>;
	using FlatInstSkn = FlatBase<true, true>;
}