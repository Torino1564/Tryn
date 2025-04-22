#include "TrynPCH.h"
#include "TexturePool.h"
#include <assimp/scene.h>

#include "win/gltfSDK.h"

namespace tryn::gfx
{
	void TexturePool::Remover::operator()(const Texture* pTexture) const
	{
		TexturePool::Get().pool.erase(pTexture->GetID());
	}

	TexturePool::Remover& TexturePool::Remover::Get()
	{
		static Remover remover;
		return remover;
	}

	std::shared_ptr<Texture> TexturePool::Resolve(const std::filesystem::path& path, std::optional<glm::vec3> scale)
	{
		auto id = Texture::GenerateID(path.string(), scale);

		const auto it = Get().pool.find(id);

		if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
		{
			auto ptr = std::shared_ptr<Texture>(new Texture(path, scale), Remover{});

			Get().pool[id] = std::weak_ptr(ptr);
			return ptr;
		}
		else
		{
			return std::shared_ptr{ it->second.lock() };
		}
	}

	std::shared_ptr<Texture> TexturePool::Resolve(const aiTexture& tex, std::optional<glm::vec3> scale)
	{
		auto id = Texture::GenerateID( tex.mFilename.C_Str(), scale);

		const auto it = Get().pool.find(id);

		if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
		{
			auto ptr = std::shared_ptr<Texture>(new Texture(tex, scale), Remover{});

			Get().pool[id] = std::weak_ptr(ptr);
			return ptr;
		}
		else
		{
			return std::shared_ptr{ it->second.lock() };
		}
	}

	std::shared_ptr<Texture> TexturePool::Resolve(const GLTFTextureData& textureData, std::optional<glm::vec3> scale)
	{
		auto id = Texture::GenerateID( textureData.name, scale);

		const auto it = Get().pool.find(id);

		if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
		{
			auto ptr = std::shared_ptr<Texture>(new Texture(textureData, scale), Remover{});

			Get().pool[id] = std::weak_ptr(ptr);
			return ptr;
		}
		else
		{
			return std::shared_ptr{ it->second.lock() };
		}
	}
}
