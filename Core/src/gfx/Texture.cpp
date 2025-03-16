#include "TrynPCH.h"
#include "Texture.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>
#include <assimp/texture.h>

namespace tryn::gfx
{
	class Deleter
	{
	public:
		Deleter(std::function<void(std::byte*)>&& deleter) : pDeleter(std::make_unique<std::function<void(std::byte*)>>(std::move(deleter))) {}
		void operator()(std::byte* bytes) const
		{
			pDeleter->operator()(bytes);
		}
		std::unique_ptr<std::function<void(std::byte*)>> pDeleter;
	};

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
		Deleter deleter([](std::byte* bytes) -> void
		{
			STBI_Close::Get().operator()(bytes);
		});

		auto& deleterRef = deleter;

		buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleterRef));
	}

	Texture::Texture(const aiTexture& tex, std::optional<glm::vec3> scale)
	{
		hasAlpha = true;

		// create buffer

		trynass(tex.mHeight != 0).msg(L"Tried to load a compressed texture, which are currently not supported!");

		const auto bufferSize = tex.mHeight * tex.mWidth * sizeof(aiTexel);
		std::byte* buffer_ = new std::byte[tex.mHeight * tex.mWidth * sizeof(aiTexel)];

		std::memcpy(buffer_, tex.pcData, bufferSize);

		this->path = tex.mFilename.C_Str();
		if (scale)
		{
			this->scale = scale;
		}
		thread_local Deleter deleter([](std::byte* bytes)
		{
			free(bytes);
		});

		buffer = std::move(std::unique_ptr<std::byte,Deleter>(buffer_, deleter));
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
