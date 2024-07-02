#pragma once
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>
#include "EntryMain.h"

#ifdef CONSOLE_ONLY
int main(int argc, char** argv)
{
	tryn::app::Main(argc, argv);
}
#else
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow)
{
	const auto argc = __argc;
	const auto wargv = __wargv;

	static constexpr auto bufferSize = 10000;
	static std::array<char, bufferSize> buffer = {};
	static std::array<char*, 100> charPtrs = {};
	auto index = 0;
	for (int i = 0; i < argc; i++)
	{
		auto tempString = tryn::utl::ToNarrow(wargv[i]);
		auto length = tempString.size();
		memcpy_s(buffer.data() + index, bufferSize - static_cast<rsize_t>(index), tempString.c_str(), length);
		charPtrs[i] = buffer.data() + index;
		index += length + 1;
	}
	tryn::app::Main(argc, charPtrs.data());
}
#endif