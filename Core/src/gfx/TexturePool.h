#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Texture.h"
#include <optional>
#include <Core/third/glm/vec3.hpp>
#include <Core/src/mem/SharedRef.h>

namespace tryn::gfx
{
	struct TexturePool
	{
	public:
		static std::shared_ptr<Texture> Resolve(const std::filesystem::path path, std::optional<glm::vec3> scale = std::nullopt)
		{
			auto id = Texture::GenerateID(path, scale);

			const auto it = Get().pool.find(id);

			auto callback = [](std::shared_ptr<Texture> pTexture) -> void
				{
					TexturePool::Get().pool.erase(pTexture->GetID());
				};

			if (it == Get().pool.end() || it != Get().pool.end() && it->second.expired())
			{
				auto ptr = std::make_shared<Texture>(path, scale);
				Get().pool[id] = std::weak_ptr<Texture>(ptr);
				return ptr;
			}
			else
			{
				return std::shared_ptr{ it->second.lock() };
			}
		}
	private:
		static TexturePool& Get()
		{
			static TexturePool singleton;
			return singleton;
		}
		std::unordered_map<std::string, std::weak_ptr<Texture>> pool;
	};
}