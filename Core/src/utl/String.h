#pragma once
#include <string>

#define toString(name) #name;

namespace tryn::utl
{
	std::wstring ToWide(const std::string& narrow);
	std::string ToNarrow(const std::wstring& wide);
}