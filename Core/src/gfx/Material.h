#pragma once
#include <Core/src/gfx/Attribute.h>
#include <Core/src/gfx/Vertex.h>
#include <concepts>
#include <span>

#include "Core/src/utl/StringHasher.h"

namespace tryn::gfx
{
	class TechniqueBase;
}

template <typename T>
concept TechniqueClass = std::derived_from<T, tryn::gfx::TechniqueBase>;

struct aiMaterial;
struct aiMesh;
struct aiScene;

namespace Microsoft::glTF
{
	struct MeshPrimitive;
	class Document;
	class Material;
}

namespace tryn::gfx
{
	class WinGLTFLoaderContext;
	class IGraphics;

	enum class TextureType : std::uint8_t
	{
		Emissive,
		Normal,
		Occlussion,
		MetallicRoughness,
		MetallicRoughnessBaseColor,
		Diffuse,
		Specular,
		Unknown
	};

	enum class AttributeType : std::uint8_t
	{
		DiffuseColor,
		SpecularColor,
		SpecularGloss,
		Unknown
	};

	class Material
	{
		friend class TechniqueBase;
	public:
		Material(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, const aiScene* pScene = nullptr, bool instanced = false, bool skinned = false);
		Material(const IGraphics& gfx, const Microsoft::glTF::Material& material, const std::filesystem::path& path, const WinGLTFLoaderContext& context, bool instanced = false, bool skinned = false);
		static std::shared_ptr<Material> MakeDefault(const IGraphics& gfx);
		VertexBuffer ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton = nullptr) const noexcept;
		VertexBuffer ExtractVertices(const Shape3D& mesh) const noexcept;
		VertexBuffer ExtractVertices(const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context, std::optional<ani::Skeleton> skeleton = std::nullopt) const noexcept;
		static IndexBuffer ExtractIndices(const aiMesh& mesh) noexcept;
		static IndexBuffer ExtractIndices(const Shape3D& mesh) noexcept;
		static IndexBuffer ExtractIndices(const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context) noexcept;
		const aiScene* pScene;

		template <typename T>
		T GetAttribute(const AttributeType attribute) const
		{
			auto& att = attributes.at(attribute);
			return att.Get<T>();
		}

		template <typename T>
		T GetAttributeOr(const AttributeType attribute, T&& orValue) const
		{
			try
			{
				auto& att = attributes.at(attribute);
				return att.Get<T>();
			}
			catch (std::exception& e)
			{
				return std::forward<T>(orValue);
			}
		}

		bool HasAttribute(AttributeType attribute) const;
		bool HasTexture(TextureType texture) const;
		std::shared_ptr<class Texture> GetTexture(TextureType texture) const;

	private:
		Material() = default;
		Material(const aiScene* pScene);
		std::unordered_map<AttributeType, Attribute> attributes;
		std::unordered_map<TextureType, std::shared_ptr<Texture>> textures;
		int selectedTechnique = -1;
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
