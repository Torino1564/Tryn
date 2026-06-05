#pragma once
#include <filesystem>
#include <optional>
#include <glm/vec3.hpp>
#include <format>
#include <Core/src/gfx/StbImageManager.h>

#include "TextureFormat.h"

struct aiTexture;

namespace tryn::gfx
{
	class Deleter
	{
	public:
		Deleter(void(*pDeleter)(std::byte*)) : pDeleter(pDeleter) {}
		Deleter() = default;
		void operator()(std::byte* bytes) const
		{
			pDeleter(bytes);
		}
		void(*pDeleter)(std::byte*) = nullptr;
	};

	class Texture
	{

	public:
		Texture(const std::filesystem::path& path);
		Texture(const aiTexture& tex);
		//Texture(const struct GLTFTextureData& textureData);
		Texture(spa::DimensionsI dimensions, TextureFormat format = TextureFormat::B8G8R8A8_UNORM);
		static constexpr std::string GenerateID(const std::string& name);
		std::string GetID() const noexcept;
		const std::byte* Data() const noexcept;
		int GetHeight() const noexcept;
		int GetWidth() const noexcept;
		int GetNumChannels() const noexcept;
		int GetRowPitch() const noexcept;
		bool HasAlpha() const noexcept;
		const std::string& GetPath() const;
		TextureFormat GetFormat() const;

	private: 
		// Data
		std::string path;
		spa::DimensionsI dimensions;
		int numChannels = 0;
		bool hasAlpha = false;
		TextureFormat format = TextureFormat::B8G8R8A8_UNORM;
		// Bytes
		std::unique_ptr<std::byte, Deleter> buffer;
	};

	constexpr std::string Texture::GenerateID(const std::string& name)
	{
		std::string id = "#Texture#";
		id += name;
		return id;
	}
}
