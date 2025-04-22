#pragma once

namespace Microsoft::glTF
{
	class Document;
	class GLTFResourceReader;
}

namespace tryn::gfx
{
	struct GLTFTextureData
	{
		size_t stride;
		size_t byteSize;
		std::vector<uint8_t> data;
		std::string name;
	};

	class WinGLTFLoaderContext
	{
	public:
		~WinGLTFLoaderContext();
		const std::unique_ptr<Microsoft::glTF::Document> pDocument;
		const std::unique_ptr<Microsoft::glTF::GLTFResourceReader> pReader;
	};

	class WinGLTFLoader
	{
	public:
		static WinGLTFLoaderContext Load(const std::filesystem::path& path);
	};
}
