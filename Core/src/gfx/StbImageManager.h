#pragma once
#include <Core/src/spa/Dimensions.h>

namespace tryn::gfx
{
	struct STBI_Close
	{
		void operator()(std::byte* image) const;

		static STBI_Close& Get()
		{
			static STBI_Close singleton;
			return singleton;
		}
	};

	class StbImageManager
	{
	public:
		
		static STBI_Close& GetClosure()
		{
			return STBI_Close::Get();
		}
		static std::byte* Load(std::string_view path, spa::DimensionsI& dimensions, int& numChannels, int desiredChannels );
		static std::byte* Load(std::span<std::byte> buffer, spa::DimensionsI& dimensions, int& numChannels, int desiredChannels );
		static int RGB_ALPHA();
	};
}