#pragma once
#include <string>

namespace tryn::utl
{
	std::wstring ToWide(const std::string& narrow);
	std::string ToNarrow(const std::wstring& wide);
}