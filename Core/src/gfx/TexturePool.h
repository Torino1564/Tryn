#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Texture.h"
#include <optional>
#include <glm/vec3.hpp>

namespace tryn::gfx
{
	struct TexturePool
	{
		struct Remover
		{
			void operator()(const Texture* pTexture) const;
			static Remover& Get();
		};

		static std::shared_ptr<Texture> Resolve(const std::filesystem::path& path);
		static std::shared_ptr<Texture> Resolve(const aiTexture& tex);
		static std::shared_ptr<Texture> Resolve(const class GLTFTextureData& textureData);

	private:
		static TexturePool& Get()
		{
			static TexturePool singleton;
			return singleton;
		}
		std::unordered_map<std::string, std::weak_ptr<Texture>> pool;
	};
}