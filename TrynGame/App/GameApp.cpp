#include "GameApp.h"

#include <Core/src/gfx/PointLight.h>

#include <Core/src/app/EntryPoint.h>
#include <Core/src/ecs/cmp/VelocityComponent.h>
#include <Core/src/ecs/cmp/PositionComponent.h>
#include <Core/src/ecs/cmp/ActiveComponent.h>
#include <Core/src/ecs/cmp/TransformComponent.h>
#include <Core/src/ecs/cmp/ModelComponent.h>
#include <Core/src/ecs/cmp/ScaleComponent.h>
#include <Core/src/ecs/cmp/RotationComponent.h>

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

	pPlayer = std::make_unique<Player>("player1", "Game/Resources/Models/PlayerModels/redCube/redCube.fbx", Gfx());

	pPointLight = std::make_unique<gfx::PointLight>(Gfx(), 0.01f);

	auto sponza = ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent>("sponza");

	sponza.GetComponent<ecs::cmp::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
	sponza.GetComponent<ecs::cmp::ModelComponent>().pModel = std::make_unique<gfx::Model>(Gfx(), "Game/Resources/Models/Sponza/sponza.obj");
	sponza.GetComponent<ecs::cmp::ScaleComponent>().scale = { 0.01f, 0.01f, 0.01f };
	sponza.GetComponent<ecs::cmp::ActiveComponent>().active = true;

	this->wnd->keyboard.DisableAutoRepeat();

	camera.GetPosition() = { 0.0f, 10.0f, 0.0f };
	camera.GetDirection() = { 0.0f, -90.0f, 0.0f };
}

void TrynGameApp::DoFrame()
{
	// Process input
	auto& playerVelocity = pPlayer->GetComponent<ecs::cmp::VelocityComponent>().velocity;
	playerVelocity = { 0.0f, 0.0f, 0.0f };
	camera.Submit(Gfx());
	pPointLight->SubmitLight(Gfx());
	pPointLight->Submit(Gfx(), camera.GetViewMatrix());
	pPointLight->ShowControls();
	
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

		while (const auto delta = wnd->mouse.ReadRawDelta())
		{
			camera.Rotate(delta->x, delta->y);
		}
	}

	camera.ShowControls();

	// Toggle 1st Person Camera
	if (const auto event = wnd->keyboard.ReadKey(); event.IsTypePress() && event.GetCode() == VK_ESCAPE)
	{
		if (wnd->IsCursorEnabled())
		{
			wnd->DisableCursor();
		}
		else
		{
			wnd->EnableCursor();
		}
	}
	camera.Update();
}
