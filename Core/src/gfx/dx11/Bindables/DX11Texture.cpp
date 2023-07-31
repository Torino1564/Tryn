#include "DX11Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <Core/third/stb_image.h>

namespace tryn::gfx
{
	dx11::DX11Texture::DX11Texture(Graphics& gfx, const std::string_view path, const uint8_t slot)
	{
		this->path = path;
		this->slot = slot;


		const char* imagePath = "path/to/your/image.png";
		int width, height, numChannels;
		unsigned char* image = stbi_load(imagePath, &width, &height, &numChannels, 0);


	}
}
