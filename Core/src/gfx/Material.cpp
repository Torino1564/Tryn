#include "TrynPCH.h"
#include "Material.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <Core/src/gfx/Render/Techniques/ForwardPhong.h>
#undef min
#undef max
#include <GLTFSDK/Document.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>

#include "IGraphics.h"
#include "Texture.h"
#include "TexturePool.h"
#include "win/gltfSDK.h"
#include <GLTFSDK/ExtensionsKHR.h>
#include <GLTFSDK/MeshPrimitiveUtils.h>

namespace tryn::gfx
{
	std::shared_ptr<Material> Material::MakeDefault(const IGraphics& gfx)
	{
		static const aiMaterial emptyMat = {};
		static const std::filesystem::path emptyPath = {};
		static auto defaultMat = std::make_shared<Material>(gfx, emptyMat, emptyPath);
		return defaultMat;
	}

	Material::Material(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, const aiScene* pScene, const bool instanced, const bool skinned)
			: pScene(pScene)
	{
		const auto rootPath = path.parent_path().string() + "\\";

		// Add textures
		aiString tempFileName;

		// Diffuse
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tempFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> pTexture;
			if (auto aiTexture = pScene->GetEmbeddedTexture(tempFileName.C_Str()))
			{
				pTexture = TexturePool::Resolve(*aiTexture);
			}
			else
			{
				pTexture = TexturePool::Resolve(rootPath + tempFileName.C_Str());
			}
			textures.insert({ TextureType::Diffuse, pTexture });
		}

		// Normal
		if (material.GetTexture(aiTextureType_NORMALS, 0, &tempFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> pTexture;
			if (auto aiTexture = pScene->GetEmbeddedTexture(tempFileName.C_Str()))
			{
				pTexture = TexturePool::Resolve(*aiTexture);
			}
			else
			{
				pTexture = TexturePool::Resolve(rootPath + tempFileName.C_Str());
			}
			textures.insert({ TextureType::Normal, pTexture });
		}

		// Specular
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &tempFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> pTexture;
			if (auto aiTexture = pScene->GetEmbeddedTexture(tempFileName.C_Str()))
			{
				pTexture = TexturePool::Resolve(*aiTexture);
			}
			else
			{
				pTexture = TexturePool::Resolve(rootPath + tempFileName.C_Str());
			}
			textures.insert({ TextureType::Specular, pTexture });
		}

		// Specular
		if (material.GetTexture(aiTextureType_EMISSIVE, 0, &tempFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> pTexture;
			if (auto aiTexture = pScene->GetEmbeddedTexture(tempFileName.C_Str()))
			{
				pTexture = TexturePool::Resolve(*aiTexture);
			}
			else
			{
				pTexture = TexturePool::Resolve(rootPath + tempFileName.C_Str());
			}
			textures.insert({ TextureType::Emissive, pTexture });
		}

		// Add Attributes
		// Diffuse Color
		{
			aiColor3D color = { 0.45f,0.45f,0.85f };
			material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
			attributes.insert({ AttributeType::DiffuseColor, Attribute::Make<glm::vec3>({color.r, color.g, color.b}) });
		}

		// Specular Color
		{
			aiColor3D color = { 0.18f,0.18f,0.18f };
			material.Get(AI_MATKEY_COLOR_SPECULAR, color);
			attributes.insert({ AttributeType::SpecularColor, Attribute::Make<glm::vec3>({color.r, color.g, color.b}) });
		}

		// Specular Gloss
		{
			float gloss = 8.0f;
			material.Get(AI_MATKEY_SHININESS, gloss);
			attributes.insert({ AttributeType::SpecularGloss, Attribute::Make<float>(gloss)});
		}
	}

	Material::Material(const IGraphics& gfx, const Microsoft::glTF::Material& material,
		const std::filesystem::path& path, const WinGLTFLoaderContext& context, bool instanced, const bool skinned)
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
				auto imgdata = context.pReader->ReadBinaryData(document, img);
			
				GLTFTextureData data = {.data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::Emissive, tex});
			}

			if (!normalTextureId.empty())
			{
				auto& normalTexture = document.textures[normalTextureId];
				auto& img = document.images[normalTexture.imageId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);
			
				GLTFTextureData data = {.byteSize = imgdata.size(), .data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::Normal, tex});
			}

			if (!occlusionTextureId.empty())
			{
				auto& occlusionTexture = document.textures[occlusionTextureId];
				auto& img = document.images[occlusionTexture.imageId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);

				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::Occlussion, tex });
			}

			if (!metallicRoughnessTextureId.empty())
			{
				auto& metallicRoughnessTexture = document.textures[metallicRoughnessTextureId];
				auto& img = document.images[metallicRoughnessTexture.imageId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);

				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::MetallicRoughness, tex });
			}

			if (!baseColorTextureId.empty())
			{
				auto& baseColorTexture = document.textures[baseColorTextureId];
				auto& img = document.images[baseColorTexture.imageId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);

				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::MetallicRoughnessBaseColor, tex });
			}
		}

		// TODO: Add mode things here regarding KHR Extensions

		// KHR Extensions
		if (material.HasExtension<Microsoft::glTF::KHR::Materials::PBRSpecularGlossiness>())
		{
			auto& specGloss = material.GetExtension<Microsoft::glTF::KHR::Materials::PBRSpecularGlossiness>();

			// Add attributes

			// diffuse color
			{
				auto color = specGloss.diffuseFactor.AsColor3();
				attributes.emplace(AttributeType::DiffuseColor, Attribute::Make<glm::vec3>({ color.r, color.g, color.b }));
			}

			// specular color
			{
				auto color = specGloss.specularFactor;
				attributes.emplace(AttributeType::SpecularColor, Attribute::Make<glm::vec3>({ color.r, color.g, color.b }));
			}

			// specular gloss
			{
				auto gloss = specGloss.glossinessFactor;
				attributes.emplace(AttributeType::SpecularGloss, Attribute::Make<float>(gloss));
			}

			// Add textures

			if (auto diffuseTextureId = specGloss.diffuseTexture.textureId; !diffuseTextureId.empty())
			{
				auto diffuseTexture = document.textures[diffuseTextureId];
				auto img = document.images[diffuseTexture.imageId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);
				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::Diffuse, tex });
			}

			if (auto specularTextureId = specGloss.specularGlossinessTexture.textureId; !specularTextureId.empty())
			{
				auto specularTexture = document.textures[specularTextureId];
				auto img = document.images[specularTexture.imageId];
				auto imgdata = context.pReader->ReadBinaryData(document, img);
				GLTFTextureData data = { .byteSize = imgdata.size(), .data = std::move(imgdata), .name = img.uri };
				auto tex = TexturePool::Resolve(data);
				// Load Texture
				textures.insert({TextureType::Specular, tex });
			}
		}
	}

	bool Material::HasAttribute(const AttributeType attribute) const
	{
		return attributes.contains(attribute);
	}

	bool Material::HasTexture(const TextureType texture) const
	{
		return textures.contains(texture);
	}

	std::shared_ptr<Texture> Material::GetTexture(const TextureType texture) const
	{
		return textures.at(texture);
	}

	Material::Material(const aiScene* pScene)
		: pScene(pScene)
	{
	}
}
