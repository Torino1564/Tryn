#include <Core/src/win/TrynWin.h>
#include <Core/src/log/Log.h>
#include <Core/src/ioc/Container.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include <Core/src/win/Boot.h>
#include <Core/src/win/IWindow.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/Gfx.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/gfx/Vertex.h>
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

	auto testSize = sizeof(glm::vec3);

	using gfx::VertexLayout;

	gfx::VertexBuffer testBuffer(gfx::VertexLayout(
		gfx::VertexLayout::VertexElement::Position3D,
		gfx::VertexLayout::VertexElement::Normal,
		gfx::VertexLayout::VertexElement::UV
		), 100);

	testBuffer[5].Attr<gfx::VertexLayout::VertexElement::Position3D>() = glm::vec3(5.0f , 1.0f , 45.0f);
	auto& testPos = testBuffer[5].Attr< gfx::VertexLayout::VertexElement::Position3D>();

	auto x = testPos.x;
	auto y = testPos.y;
	auto z = testPos.z;

	auto& testPos2 = testBuffer[5].Attr< gfx::VertexLayout::VertexElement::Position3D>();

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