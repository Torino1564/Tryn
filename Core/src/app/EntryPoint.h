#pragma once
#include <Core/src/app/App.h>
#include <Core/src/app/Initialization.h>
#include <Core/src/utl/Assert.h>

extern tryn::app::App* tryn::app::CreateApp(int argc, wchar_t** argv);

namespace tryn::app
{
	int Main(int argc, wchar_t** argv)
	{
		BootCore();
		App* app = tryn::app::CreateApp(argc, argv);
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

		delete app;
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
	wchar_t** argv = __wargv;
	tryn::app::Main(argc, argv);
}
#endif