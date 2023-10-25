#pragma once
#include <Core/src/app/App.h>
#include <Core/src/app/Initialization.h>
#include <Core/src/utl/Assert.h>

extern tryn::app::App* tryn::app::CreateApp(int argc, char** argv);

namespace tryn::app
{
	int Main(int argc, char** argv)
	{
		BootCore();
		std::unique_ptr<App> app(tryn::app::CreateApp(argc, argv));
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
	tryn::app::Main(__argc, __argv);
}
#endif