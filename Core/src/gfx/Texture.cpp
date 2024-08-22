#include "TrynPCH.h"
#include "Texture.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>

namespace tryn::gfx
{
	Texture::Texture(const std::filesystem::path& path, std::optional<glm::vec3> scale)
	{
		auto texture = StbImageManager::Load(path.string(), dimensions, numChannels, StbImageManager::RGB_ALPHA());

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

		buffer = std::move(std::unique_ptr<std::byte,STBI_Close>(texture, STBI_Close::Get()));
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
		return dimensions.height;
	}
	int Texture::GetWidth() const noexcept
	{
		return dimensions.width;
	}
	int Texture::GetNumChannels() const noexcept
	{
		return StbImageManager::RGB_ALPHA();
	}
	int Texture::GetRowPitch() const noexcept
	{
		return dimensions.width * StbImageManager::RGB_ALPHA();
	}
	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
}
