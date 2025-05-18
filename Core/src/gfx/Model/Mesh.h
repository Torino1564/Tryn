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
		Mesh();
		~Mesh() override = default;
		[[nodiscard]] std::string GetTag() const;
		[[nodiscard]] virtual MeshType Type() const = 0;
		virtual void EnableOrAddTechnique(const IGraphics& gfx, utl::UUID_t techniqueUUID, std::span<uint16_t> materialIndex = { std::array{(uint16_t)0}.begin(), 1 }) = 0;
		virtual void AddTechnique(const IGraphics& gfx, utl::UUID_t techniqueUUID, std::span<uint16_t> materialIndex = { std::array{(uint16_t)0}.begin(), 1 }, bool enabled = true) = 0;

	protected:
		std::string tag;
	};
}
