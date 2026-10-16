
#include "Material.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <Core/src/gfx/Render/Techniques/ForwardPhong.h>
#undef min
#undef max

#include "IGraphics.h"
#include "Texture.h"
#include "TexturePool.h"
#include <fastgltf/core.hpp>

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

	Material::Material(const IGraphics& gfx, fastgltf::Asset& gltf, const fastgltf::Material& material, const std::filesystem::path& path, bool instanced, bool skinned)
		: pScene(nullptr)
	{
		// Fill textures:
		auto& emissiveTexture = material.emissiveTexture;
		auto& normalTexture = material.normalTexture;
		auto& occlusionTexture = material.occlusionTexture;
		auto& metallicRoughnessTexture = material.pbrData.metallicRoughnessTexture;
		auto& baseColorTexture = material.pbrData.baseColorTexture;

		// Load Textures

		if (emissiveTexture)
		{
			auto tex = TexturePool::Resolve(gltf, gltf.images[emissiveTexture.value().textureIndex]);
			// Load Texture
			textures.insert({ TextureType::Emissive, tex });
		}
		if (normalTexture)
		{
			auto tex = TexturePool::Resolve(gltf, gltf.images[normalTexture.value().textureIndex]);
			// Load Texture
			textures.insert({ TextureType::Normal, tex });
		}
		if (occlusionTexture)
		{
			auto tex = TexturePool::Resolve(gltf, gltf.images[occlusionTexture.value().textureIndex]);
			// Load Texture
			textures.insert({ TextureType::Occlussion, tex });
		}
		if (metallicRoughnessTexture)
		{
			auto tex = TexturePool::Resolve(gltf, gltf.images[metallicRoughnessTexture.value().textureIndex]);
			// Load Texture
			textures.insert({ TextureType::MetallicRoughness, tex });
		}
		if (baseColorTexture)
		{
			auto tex = TexturePool::Resolve(gltf, gltf.images[baseColorTexture.value().textureIndex]);
			// Load Texture
			textures.insert({ TextureType::MetallicRoughnessBaseColor, tex });
		}

		if (material.specular)
		{
			// Add attributes
			// specular color
			{
				auto color = material.specular->specularColorFactor;
				attributes.emplace(AttributeType::SpecularColor, Attribute::Make<glm::vec3>({ color.x(), color.y(), color.z()}));
			}
			// specular gloss
			{
				auto gloss = material.specular->specularFactor;
				attributes.emplace(AttributeType::SpecularGloss, Attribute::Make<float>(gloss));
			}
			// Load Texture
			if (material.specular->specularColorTexture)
			{
				auto tex = TexturePool::Resolve(gltf, gltf.images[material.specular->specularColorTexture.value().textureIndex]);
				textures.insert({ TextureType::Specular, tex });
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
	{
	}
}
