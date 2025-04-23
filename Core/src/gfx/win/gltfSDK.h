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
		static void Load(const std::filesystem::path& path, const std::function<void(const WinGLTFLoaderContext&)>& process);
	};
}
