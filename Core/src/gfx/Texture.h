#pragma once
#include <filesystem>
#include <optional>
#include <Core/third/glm/vec3.hpp>
#include <format>

namespace tryn::gfx
{
	class Texture
	{
	public:
		Texture(const std::filesystem::path& path, std::optional<glm::vec3> scale = std::nullopt);
		~Texture();
		static constexpr std::string GenerateID(const std::filesystem::path& path, std::optional<glm::vec3> scale = std::nullopt)
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
		std::string GetID() const noexcept
		{
			return GenerateID(path, scale);
		}
		const std::byte* Data() const noexcept;
		int GetHeight() const noexcept;
		int GetWidth() const noexcept;
		int GetNumChannels() const noexcept;
		int GetRowPitch() const noexcept;
		bool HasAlpha() const noexcept;
	private:
		// Data
		std::string path;
		glm::vec3 scale = { 1.0f,1.0f,1.0f };
		int height = 0;
		int width = 0;
		int numChannels = 0;
		bool hasAlpha = false;
		// Bytes
		std::byte* buffer;
	};
}