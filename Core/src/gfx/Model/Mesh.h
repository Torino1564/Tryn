#pragma once
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	enum class MeshType
	{
		Static,
		Boned,
		GLTF
	};

	class Mesh : public Drawable
	{
	public:
		~Mesh() override = default;
		[[nodiscard]] std::string GetTag() const;
		[[nodiscard]] virtual MeshType Type() const = 0;

	protected:
		std::string tag;
	};
}
