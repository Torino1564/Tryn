
#include "EntryMain.h"
#define CLI11_COMPILE 1
#include <CLI/CLI.hpp>
#include <Core/src/app/Initialization.h>
#include <Core/src/app/Globals.h>
#include <Core/src/win/TrynWin.h>

namespace tryn::app
{
	int Main(int argc, char** argv)
	{
		BootCore();
		std::unique_ptr<App> app(CreateApp(argc, argv));

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
		catch (std::exception& e)
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