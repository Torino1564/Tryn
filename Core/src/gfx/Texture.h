#pragma once
#include <filesystem>
#include <optional>
#include <Core/third/glm/vec3.hpp>
#include <format>
#include <Core/src/gfx/StbImageManager.h>

namespace tryn::gfx
{
	class Texture
	{
		
	public:
		Texture(const std::filesystem::path& path, std::optional<glm::vec3> scale = std::nullopt);
		static constexpr std::string GenerateID(const std::filesystem::path& path, std::optional<glm::vec3> scale = std::nullopt);
		std::string GetID() const noexcept;
		const std::byte* Data() const noexcept;
		int GetHeight() const noexcept;
		int GetWidth() const noexcept;
		int GetNumChannels() const noexcept;
		int GetRowPitch() const noexcept;
		bool HasAlpha() const noexcept;
	private:
		// Data
		std::string path;
		std::optional<glm::vec3> scale;
		spa::DimensionsI dimensions;
		int numChannels = 0;
		bool hasAlpha = false;
		// Bytes
		std::unique_ptr<std::byte, STBI_Close> buffer;
	};

	constexpr std::string Texture::GenerateID(const std::filesystem::path& path, std::optional<glm::vec3> scale)
	{
		std::string id = "#Texture#";
		id += path.string();
		if (scale)
		{
			id += "#Scale:";
			id += std::format("X:{}Y:{}Z:{}", scale->x, scale->y, scale->z);
		}
		return id;
	}
}
