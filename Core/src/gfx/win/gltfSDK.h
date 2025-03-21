#pragma once

namespace Microsoft::glTF
{
	class Document;
}

namespace tryn::gfx
{
	class WinGLTFLoader
	{
	public:
		static Microsoft::glTF::Document Load(const std::filesystem::path& path);
	};
}
