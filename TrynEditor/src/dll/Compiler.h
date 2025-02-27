#pragma once
#include <filesystem>

namespace tryn::ed
{
	class Compiler
	{
	public:
		void CompileToDLL(const std::string& sourceFile);
	};
}

