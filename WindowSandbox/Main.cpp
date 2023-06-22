#include <Core/src/win/TrynWin.h>
#include <Core/src/log/Log.h>
#include <Core/src/ioc/Container.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include <Core/src/win/Boot.h>
#include <Core/src/win/IWindow.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/Gfx.h>
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
		return std::make_shared<log::SeverityLevelPolicy>(log::Level::Info);
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

	while (!window->IsClosing())
	{
		gfx->BeginFrame();

		gfx->ClearBuffer(0.2, 0.5, 0.7);

		gfx->EndFrame();
	}

	return 0;
}