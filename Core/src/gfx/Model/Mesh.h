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
		bool IsParentMesh() const;
		virtual void SubmitBoned(const IGraphics& gfx, const glm::mat4& finalTransform, std::span<const glm::mat4> boneTransforms) = 0;
		const std::vector<std::shared_ptr<Mesh>>& GetChildren() const;
		std::vector<std::shared_ptr<Mesh>>& GetChildren();
	protected:
		bool isParentMesh = false;
		std::vector<std::shared_ptr<Mesh>> children;
		std::string tag;
	};
}
