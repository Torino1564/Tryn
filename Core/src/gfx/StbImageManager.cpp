
#include "StbImageManager.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <Core/third/stb_image/stb_image.h>
#include <Core/third/stb_image/stb_image_write.h>
#include "Core/src/utl/Assert.h"

namespace tryn::gfx
{
	void STBI_Close::operator()(std::byte* image) const
	{
		trynass(image).msg(L"Tried to free an empty stb image!").ex();
		stbi_image_free(image);
	}

	std::byte* StbImageManager::Load(std::string_view path, spa::DimensionsI& dimensions, int& numChannels,
		int desiredChannels)
	{
		return (std::byte*)stbi_load(path.data(), &dimensions.width, &dimensions.height, &numChannels, desiredChannels);
	}

	std::byte* StbImageManager::Load(std::span<std::byte> buffer, spa::DimensionsI& dimensions, int& numChannels,
		int desiredChannels)
	{
		return (std::byte*)stbi_load_from_memory((stbi_uc*)buffer.data(), buffer.size() ,&dimensions.width, &dimensions.height, &numChannels, desiredChannels);
	}

	int StbImageManager::RGB_ALPHA()
	{
		return STBI_rgb_alpha;
	}
}
