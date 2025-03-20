#pragma once

namespace tryn::gfx
{
	class WinGLTFLoader
	{
	public:
		static void Load(const std::filesystem::path& path);
	};
}