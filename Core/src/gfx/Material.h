#pragma once
#include <core/src/gfx/Render/Technique.h>
#include <Core/src/gfx/Vertex.h>
#include <concepts>
#include <span>

template <typename T>
concept TechniqueClass = std::derived_from<T, class tryn::gfx::TechniqueBase>;

struct aiMaterial;
struct aiMesh;
struct aiScene;

namespace Microsoft::glTF
{
	class Document;
	class Material;
}

namespace tryn::gfx
{
	class IGraphics;
	class Material
	{
		friend class TechniqueBase;
	public:
		template <TechniqueClass... T>
		static Material Make(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, const aiScene* pScene = nullptr, bool instanced = false, bool skinned = false);
		Material(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, std::span<const utl::UUID_t> techniqueUUIDs, const aiScene* pScene = nullptr, bool instanced = false, bool skinned = false);
		Material(const IGraphics& gfx, const Microsoft::glTF::Material& material, const std::filesystem::path& path, std::span<const utl::UUID_t> techniqueUUIDs, const Microsoft::glTF::Document& document, bool instanced = false, bool skinned = false);
		static Material MakeDefault(const IGraphics& gfx);
		VertexBuffer ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton = nullptr) const noexcept;
		static IndexBuffer ExtractIndices(const aiMesh& mesh) noexcept;
		VertexBuffer ExtractVertices(const Shape3D& mesh) const noexcept;
		IndexBuffer ExtractIndices(const Shape3D& mesh) const noexcept;
		std::vector<std::shared_ptr<TechniqueBase>> GetTechniques() const noexcept;
		void AddTechnique(utl::UUID_t techniqueUUID, const IGraphics& gfx, const aiMaterial& material, const std::string& path, bool instanced, bool skinned);
		const aiScene* pScene;

	private:
		Material() = default;
		Material(const aiScene* pScene);
		VertexLayout vLayout;
		std::vector<std::shared_ptr<class Attribute>> pAttributes;
		std::vector<std::shared_ptr<class Texture>> pTextures;
		std::vector<std::shared_ptr<TechniqueBase>> pTechniques;
		std::string name;

	};

	template <TechniqueClass ... T>
	Material Material::Make(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path,
		const aiScene* pScene, bool instanced, bool skinned)
	{
		static constexpr auto arr = {ZT_TYPE_UUID(T) ...};
		return Material(gfx, material, path, std::span{arr}, pScene, instanced, skinned);
	}
}
