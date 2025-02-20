#pragma once
#include <filesystem>

namespace tryn::ed
{
	class Compiler
	{
		void CompileToDLL(const std::filesystem::path& path, std::string_view fileName);
	};	
}

