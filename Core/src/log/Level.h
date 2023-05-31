#pragma once
#include <string>

namespace tryn::log
{
	enum class Level
	{
		None,
		Fatal,
		Error,
		Warn,
		Info,
		Debug,
		Verbose,
	};

	std::wstring GetLevelName(Level lv);
}