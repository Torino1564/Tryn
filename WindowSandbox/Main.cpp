#include <Core/src/win/TrynWin.h>
#include <Core/src/log/Log.h>
#include <Core/src/ioc/Container.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include <Core/src/win/Boot.h>
#include <Core/src/win/IWindow.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/Gfx.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/utl/Timer.h>
#include <iostream>
#include <array>
#include <memory>
#include <format>
#include <ranges>
#include <vector>

using namespace tryn;
using namespace std::chrono_literals;
namespace rn = std::ranges;
namespace vi = rn::views;

void Boot()
{
	log::Boot();

	ioc::Get().Register<log::ISeverityLevelPolicy>([] {
		return std::make_shared<log::SeverityLevelPolicy>(log::Level::Verbose);
		});

	win::Boot();
	gfx::Boot();
}

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow)
{
	Boot();

	auto window = ioc::Get().Resolve<win::IWindow>( );
	window->SetTitle(L"Test WindowApp");

	auto gfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{window->GetClientDimensions().width , window->GetClientDimensions().height, window->GetHandle()} );

	try {

		while (!window->IsClosing())
		{
			gfx->BeginFrame();
			gfx->ClearBuffer(0.0f, 0.0f, 0.0f);
			gfx->DrawTriangle();
			gfx->EndFrame();
		}

	}
	catch (utl::BufferedException e)
	{
		MessageBoxA(nullptr, e.what(), 0u, MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Unknown error", 0u, MB_OK | MB_ICONEXCLAMATION);
	}

	return 0;
}