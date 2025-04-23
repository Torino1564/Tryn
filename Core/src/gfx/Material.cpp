#include "TrynPCH.h"
#include "Material.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <Core/src/gfx/Render/Techniques/ForwardPhong.h>
#include <GLTFSDK/Document.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>

#include "IGraphics.h"
#include "Texture.h"
#include "TexturePool.h"
#include "win/gltfSDK.h"

namespace tryn::gfx
{
	Material Material::MakeDefault(const IGraphics& gfx)
	{
		static const aiMaterial emptyMat = {};
		static const std::filesystem::path emptyPath = {};
		return Make<ForwardPhong>(gfx, emptyMat, emptyPath);
	}

	Material::Material(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path,
		std::span<const utl::UUID_t> techniqueUUIDs, const aiScene* pScene, const bool instanced, const bool skinned)
			: pScene(pScene)
	{
		const auto rootPath = path.parent_path().string() + "\\";

		if (techniqueUUIDs.size() == 0)
		{
			AddTechnique(ZT_TYPE_UUID(ForwardPhong), gfx, material, rootPath, instanced, skinned);
		}

		for (auto techniqueUUID : techniqueUUIDs)
		{
			AddTechnique(techniqueUUID, gfx, material, rootPath, instanced, skinned);
		}

	}

	Material::Material(const IGraphics& gfx, const Microsoft::glTF::Material& material,
		const std::filesystem::path& path, std::span<const utl::UUID_t> techniqueUUIDs,
		const WinGLTFLoaderContext& context, bool instanced, const bool skinned)
			: pScene(nullptr)
	{
		auto& document = *context.pDocument;
		const auto rootPath = path.parent_path().string() + "\\";

		// Fill textures:
		auto emissiveTextureId = material.emissiveTexture.textureId;
		auto normalTextureId = material.normalTexture.textureId;
		auto occlusionTextureId =	material.occlusionTexture.textureId;
		auto metallicRoughnessTextureId =	material.metallicRoughness.metallicRoughnessTexture.textureId;
		auto baseColorTextureId =	material.metallicRoughness.baseColorTexture.textureId;

		auto filename = path.filename().string();

		// Add Textures
		{
			if (!emissiveTextureId.empty())
			{
				auto& emissiveTexture = document.textures[emissiveTextureId];
				auto& img = document.images[emissiveTexture.imageId];
				auto bufferView = document.bufferViews[img.bufferViewId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);
			
				GLTFTextureData data = {.data = std::move(imgdata), .name = filename + "emissiveTexture"};
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({"emissiveTexture", tex});
			}

			if (!normalTextureId.empty())
			{
				auto& normalTexture = document.textures[normalTextureId];
				auto& img = document.images[normalTexture.imageId];
				auto bufferView = document.bufferViews[img.bufferViewId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);
			
				GLTFTextureData data = {.byteSize = imgdata.size(), .data = std::move(imgdata), .name = filename + "normalTexture" };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({"normalTexture", tex});
			}

			if (!occlusionTextureId.empty())
			{
				auto& occlusionTexture = document.textures[occlusionTextureId];
				auto& img = document.images[occlusionTexture.imageId];
				auto bufferView = document.bufferViews[img.bufferViewId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);

				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = filename + "occlusionTexture" };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({ "occlusionTexture", tex });
			}

			if (!metallicRoughnessTextureId.empty())
			{
				auto& metallicRoughnessTexture = document.textures[metallicRoughnessTextureId];
				auto& img = document.images[metallicRoughnessTexture.imageId];
				auto bufferView = document.bufferViews[img.bufferViewId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);

				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = filename + "metallicRoughnessTexture" };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({ "metallicRoughnessTexture", tex });
			}

			if (!baseColorTextureId.empty())
			{
				auto& baseColorTexture = document.textures[baseColorTextureId];
				auto& img = document.images[baseColorTexture.imageId];
				auto bufferView = document.bufferViews[img.bufferViewId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);

				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = filename + "baseColorTexture" };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({ "baseColorTexture", tex });
			}
		}

		// Add Attributes

		// diffuse color
		{
			auto color = material.metallicRoughness.baseColorFactor.AsColor3();
			attributes.emplace("diffuseColor", Attribute::Make<glm::vec3>("diffuseColor", {color.r, color.g, color.b}));
		}


		//if (techniqueUUIDs.size() == 0)
		//{
		//	AddTechnique(ZT_TYPE_UUID(ForwardPhong), gfx, material, rootPath, instanced, skinned);
		//}

		//for (auto techniqueUUID : techniqueUUIDs)
		//{
		//	AddTechnique(techniqueUUID, gfx, material, rootPath, instanced, skinned);
		//}
	}

	VertexBuffer Material::ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton) const noexcept
	{
		return { vLayout, mesh, skeleton};
	}
	IndexBuffer Material::ExtractIndices(const aiMesh& mesh) noexcept
	{
		std::vector<uint32_t> indices;
		indices.resize(mesh.mNumFaces * 3);

		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& triangle = mesh.mFaces[i];
			indices[3 * i] = triangle.mIndices[0];
			indices[(3 * i) + 1] = triangle.mIndices[1];
			indices[(3 * i) + 2] = triangle.mIndices[2];
		}
		return { indices };
	}

	VertexBuffer Material::ExtractVertices(const Shape3D& mesh) const noexcept
	{
		return { vLayout, mesh};
	}

	IndexBuffer Material::ExtractIndices(const Shape3D& mesh) const noexcept
	{
		return IndexBuffer(mesh.Indices());
	}

	std::vector<std::shared_ptr<TechniqueBase>> Material::GetTechniques() const noexcept
	{
		return pTechniques;
	}

	void Material::AddTechnique(utl::UUID_t techniqueUUID, const IGraphics& gfx, const aiMaterial& material,
		const std::string& path, bool instanced, bool skinned)
	{
		pTechniques.push_back(TechniquePool::ConstructTechnique(techniqueUUID, *this, material, gfx, path,  instanced, skinned));
	}

	bool Material::HasAttribute(const std::string& name) const
	{
		return attributes.contains(name);
	}

	Material::Material(const aiScene* pScene)
		: pScene(pScene)
	{
	}
}
