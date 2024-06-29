#pragma once
#include <Core/src/app/App.h>
#include <Core/src/app/Initialization.h>
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>
#include <Core/src/app/Globals.h>
#include <CLI/CLI.hpp>

extern tryn::app::App* tryn::app::CreateApp(int argc, char** argv);

namespace tryn::app
{
	int Main(int argc, char** argv)
	{
		BootCore();
		std::unique_ptr<App> app(tryn::app::CreateApp(argc, argv));

		// Init command line params
		CLI::App cliApp("A multi API game engine", "Tryn Core");

		cliApp.add_flag("--singleThreadedRenderer, -s", gbl::configs.singleThreadedRendeder, "forces singlethreaded renderer");
		cliApp.add_option("--numRenderWorkers, -r", gbl::configs.numRenderWorkers, "specify the number of render workers. Default value is 3");

		CLI11_PARSE(cliApp, argc, argv);
		
		trynass_msg(app, L"Application is null. Failed to run the client CreateApp function");
		try
		{
			app->Go();
		}
		catch (utl::BufferedException& e)
		{
			MessageBoxA(nullptr, e.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
		}
		catch (...)
		{
			MessageBoxA(nullptr, "Unknown error", nullptr, MB_OK | MB_ICONEXCLAMATION);
		}

		return 0;
	}
}
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