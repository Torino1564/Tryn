
#include "TexturePool.h"
#include <assimp/scene.h>

#include <fastgltf/core.hpp>

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

	std::shared_ptr<Texture> TexturePool::Resolve(const std::filesystem::path& path)
	{
		auto id = Texture::GenerateID(path.string());

		const auto it = Get().pool.find(id);

		if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
		{
			auto ptr = std::shared_ptr<Texture>(new Texture(path), Remover{});

			Get().pool[id] = std::weak_ptr(ptr);
			return ptr;
		}
		else
		{
			return std::shared_ptr{ it->second.lock() };
		}
	}

	std::shared_ptr<Texture> TexturePool::Resolve(const aiTexture& tex)
	{
		auto id = Texture::GenerateID( tex.mFilename.C_Str());

		const auto it = Get().pool.find(id);

		if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
		{
			auto ptr = std::shared_ptr<Texture>(new Texture(tex), Remover{});

			Get().pool[id] = std::weak_ptr(ptr);
			return ptr;
		}
		else
		{
			return std::shared_ptr{ it->second.lock() };
		}
	}

	std::shared_ptr<Texture> TexturePool::Resolve(class fastgltf::Asset& gltf, class fastgltf::Image& image)
	{
		auto id = Texture::GenerateID(image.name.data());

		const auto it = Get().pool.find(id);

		if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
		{
			auto ptr = std::shared_ptr<Texture>(new Texture(gltf, image), Remover{});

			Get().pool[id] = std::weak_ptr(ptr);
			return ptr;
		}
		else
		{
			return std::shared_ptr{ it->second.lock() };
		}
	}
}
