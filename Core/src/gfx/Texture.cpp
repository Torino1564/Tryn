#include "TrynPCH.h"
#include "Texture.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>
#include <Core/third/stb_image/stb_image.h>

namespace tryn::gfx
{
	void Texture::STBI_Close::operator()(std::byte* image)
	{
		stbi_image_free(image);
	}

	Texture::STBI_Close& Texture::STBI_Close::Get()
	{
		static STBI_Close stbi_close;
		return stbi_close;
	}

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

	std::string Texture::GetID() const noexcept
	{
		return GenerateID(path, scale);
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
		return STBI_rgb_alpha;
	}
	int Texture::GetRowPitch() const noexcept
	{
		return width * STBI_rgb_alpha;
	}
	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
}
