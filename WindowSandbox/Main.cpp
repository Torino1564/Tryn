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
#include "TestApp.h"
#include <iostream>
#include <array>
#include <memory>
#include <format>
#include <ranges>
#include <vector>
#include <Core/src/gfx/Assimp.h>
#include <fstream>
#include <Core/src/gfx/Technique/Technique.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>


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

	auto window = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{.size = spa::DimensionsI{ .width = (1280), .height = (720) } });
	window->SetTitle(L"Test WindowApp");

	auto gfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{window->GetClientDimensions().width, window->GetClientDimensions().height, window->GetHandle()});


	auto tech = gfx::Technique::Resolve<gfx::Techniques::PhongFlatColor>(*gfx);


	TestApp app(window, gfx);

	try
	{
		app.Go();
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