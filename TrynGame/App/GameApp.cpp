#include "GameApp.h"

#include <Core/src/app/EntryPoint.h>
#include <Core/src/ecs/cmp/VelocityComponent.h>
#include <Core/src/ecs/cmp/PositionComponent.h>

#include <TrynGame/Game/Core/Player.h>

app::App* app::CreateApp(int argc, char** argv)
{
	auto window = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{ .size = spa::DimensionsI{.width = (1280), .height = (720) } });
	window->SetTitle(L"TrynGame");

	ioc::Get().Register<log::ISeverityLevelPolicy>([]
		{
			return std::make_shared<log::SeverityLevelPolicy>(
#ifdef _DEBUG
				log::Level::Warn
#else
				log::Level::Info
#endif
			);
		});

	auto gfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{ window->GetClientDimensions().width, window->GetClientDimensions().height, window->GetHandle() });

	auto pApp = new TrynGameApp(window, gfx);

	return pApp;
}

TrynGameApp::TrynGameApp(std::shared_ptr<tryn::win::IWindow> pWindow, std::shared_ptr<tryn::gfx::IGraphics> pGraphics)
{
	this->wnd = pWindow;
	this->gfx = pGraphics;

	pPlayer = std::make_unique<Player>("player1", "C:/dev/Tryn/TrynGame/Game/Resources/Models/PlayerModels/redCube/redCube.fbx", Gfx());

	camera.GetPosition() = { 0.0f, 10.0f, 0.0f };
	camera.GetDirection() = { 0.0f, -90.0f, 0.0f };
}

void TrynGameApp::DoFrame()
{
	// Process input

	auto& playerVelocity = pPlayer->GetComponent<ecs::cmp::VelocityComponent>().velocity;

	if (!wnd->IsCursorEnabled())
	{
		if (wnd->keyboard.IsKeyPressed('A'))
		{
			playerVelocity = { -1.0f,0.0f,0.0f };
			camera.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd->keyboard.IsKeyPressed('S'))
		{
			playerVelocity = { 0.0f,0.0f,-1.0f };
			camera.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd->keyboard.IsKeyPressed('D'))
		{
			playerVelocity = { 1.0f,0.0f,0.0f };
			camera.Translate({ dt,0.0f,0.0f });
		}
		if (wnd->keyboard.IsKeyPressed('W'))
		{
			playerVelocity = { 1.0f,0.0f,1.0f };
			camera.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd->keyboard.IsKeyPressed(VK_SPACE))
		{
			camera.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd->keyboard.IsKeyPressed(VK_MENU))
		{
			camera.Translate({ 0.0f,-dt,0.0f });
		}
		if (wnd->keyboard.IsKeyPressed(VK_UP))
		{
			camera.Rotate(0, dt);
		}
		if (wnd->keyboard.IsKeyPressed(VK_DOWN))
		{
			camera.Rotate(0, -dt);
		}
		if (wnd->keyboard.IsKeyPressed(VK_LEFT))
		{
			camera.Rotate(-dt, 0.0f);
		}
		if (wnd->keyboard.IsKeyPressed(VK_RIGHT))
		{
			camera.Rotate(dt, 0.0f);
		}

		camera.GetPosition() = pPlayer->GetComponent<ecs::cmp::PositionComponent>().position;

		while (const auto delta = wnd->mouse.ReadRawDelta())
		{
			camera.Rotate(delta->x, delta->y);
		}

		
	}
}
