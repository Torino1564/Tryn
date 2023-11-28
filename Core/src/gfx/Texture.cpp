#include "Texture.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>

namespace tryn::gfx
{
	Texture::Texture(const std::filesystem::path& path, std::optional<glm::vec3> scale)
	{
		auto texture = stbi_load(path.string().c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

		trynass_msg(texture != nullptr, utl::ToWide(std::format("The specified file could not be loaded! File: {}", path.string().c_str())));

		if (numChannels == 4)
		{
			hasAlpha = true;
		}

		this->path = path.string();
		if (scale)
		{
			this->scale = scale;
		}

		buffer = std::move(std::unique_ptr<std::byte,STBI_Close>(reinterpret_cast<std::byte*>(texture), STBI_Close::Get()));
	}
	const std::byte* Texture::Data() const noexcept
	{
		return buffer.get();
	}
	int Texture::GetHeight() const noexcept
	{
		return height;
	}
	int Texture::GetWidth() const noexcept
	{
		return width;
	}
	int Texture::GetNumChannels() const noexcept
	{
		return numChannels;
	}
	int Texture::GetRowPitch() const noexcept
	{
		return width * numChannels;
	}
	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
}
