#include "GameApp.h"
#include <Engine/Render/RenderGraph.h>
#include <TrynGame/Game/Core/Player.h>
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/win/IWindow.h>

TrynGameApp::TrynGameApp(std::shared_ptr<tryn::win::IWindow> pWindow, std::shared_ptr<tryn::gfx::IGraphics> pGraphics)
	: App(pWindow, pGraphics)
{
	Gfx().SetRenderGraph(std::make_unique<TrynGameRenderGraph>(Gfx()));

	pPlayer = std::make_unique<Player>("player1", "Game/Resources/Models/PlayerModels/ShinySphere/ShinySphere.obj", Gfx());

	pPlayer->GetComponent<ecs::cmp::PositionComponent>().position = {10.0f, 10.0f, 10.0f};

	pPointLight = std::make_unique<gfx::PointLight>(Gfx(), 0.01f);

	{
		auto pTestEnt = std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent,
		ecs::cmp::PointLightComponent>("light"));
	}

	entities.emplace_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent,
		ecs::cmp::PointLightComponent>("light")));

	const auto& pLight = entities.back();

	pLight->GetComponent<ecs::cmp::PositionComponent>().position = { 0.0f, 20.0f, 0.0f };
	pLight->GetComponent<ecs::cmp::ModelComponent>().pModel = gfx::Model::Make<gfx::FlatBase>(Gfx(), "Game/Resources/Models/sphere.obj");
	pLight->GetComponent<ecs::cmp::ActiveComponent>().active = true;
	pLight->GetComponent<ecs::cmp::PointLightComponent>().parameters = gfx::PointLightParameters{
		.ambient = {0.1f, 0.1f, 0.1f},
		.diffuseColor = glm::normalize(glm::vec3{1.0f, 1.0f, 1.0f}),
		.diffuseIntensity = 1.0f,
		.constantAtt = 1.0f,
		.linearAtt = 0.045f,
		.quadraticAtt = 0.0075f
	};
	pLight->GetComponent<ecs::cmp::ScaleComponent>().scale = {1.0f, 1.0f, 1.0f};

	entities.emplace_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent>("sponza")));

	auto& sponza = *entities.back();

	sponza.GetComponent<ecs::cmp::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
	sponza.GetComponent<ecs::cmp::ModelComponent>().pModel = std::make_unique<gfx::Model>(Gfx(), "Game/Resources/Models/Sponza/sponza.obj");
	sponza.GetComponent<ecs::cmp::ScaleComponent>().scale = { 0.01f, 0.01f, 0.01f };
	sponza.GetComponent<ecs::cmp::ActiveComponent>().active = false;

	entities.emplace_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent>("TestPlane")));

	auto& plane = *entities.back();

	plane.GetComponent<ecs::cmp::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
	plane.GetComponent<ecs::cmp::ModelComponent>().pModel = gfx::Model::Make(Gfx(), "Game/Resources/Models/Environments/TestPlane.obj");
	plane.GetComponent<ecs::cmp::ScaleComponent>().scale = { 10.0f, 10.0f, 10.0f };
	plane.GetComponent<ecs::cmp::ActiveComponent>().active = true;

	auto& pModelWatch = plane.GetComponent<ecs::cmp::ModelComponent>().pModel;

	this->wnd->keyboard.DisableAutoRepeat();

	camera.GetPosition() = { 0.0f, 10.0f, 0.0f };
	camera.GetDirection() = { 0.0f, 0.0f, 0.0f };

	player.SetPitch(75.0f);
	player.SetYaw(45.0f);
	player.GetPosition() = { 0.0f, 22.5f, 0.0f };
}

void TrynGameApp::DoFrame()
{
	for (auto& view : ecs::ComponentManager::ComponentViewVector())
	{
		view.Func<0>();
	}

	// Process input
	auto& playerVelocity = pPlayer->GetComponent<ecs::cmp::VelocityComponent>().velocity;
	playerVelocity = { 0.0f, 0.0f, 0.0f };
	pActiveCamera->Submit(Gfx());

	for (const auto& pEntity : entities)
	{
		pEntity->SpawnControlWindow();
	}
	
	if (state == Mode::Player)
	{
		auto& playerCameraPosition = player.GetPosition();
		auto& playerObjectPosition = pPlayer->GetComponent<ecs::cmp::PositionComponent>().position;
		playerCameraPosition.x = playerObjectPosition.x;
		playerCameraPosition.z = playerObjectPosition.z;

		if (wnd->keyboard.IsKeyPressed('A'))
		{
			playerVelocity.x = -5.0f;
		}
		if (wnd->keyboard.IsKeyPressed('D'))
		{
			playerVelocity.x = 5.0f;
		}
		if (wnd->keyboard.IsKeyPressed('W'))
		{
			playerVelocity.z = 5.0f;
		}
		if (wnd->keyboard.IsKeyPressed('S'))
		{
			playerVelocity.z = -5.0f;
		}
	}


	if (state == Mode::Ghost)
	{
		if (wnd->keyboard.IsKeyPressed('A'))
		{
			camera.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd->keyboard.IsKeyPressed('S'))
		{
			camera.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd->keyboard.IsKeyPressed('D'))
		{
			camera.Translate({ dt,0.0f,0.0f });
		}
		if (wnd->keyboard.IsKeyPressed('W'))
		{
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

	pActiveCamera->ShowControls();
	pActiveCamera->ShowDebugInfo();
	
	// State Switching
	StateMachine();
	ShowStateInfo();

	pActiveCamera->Update();
}

void TrynGameApp::StateMachine()
{
	if (const auto event = wnd->keyboard.ReadKey(); event.IsTypePress() && ((event.GetCode() == VK_ESCAPE) || (event.GetCode() == 'P')))
	{
		if (state == Mode::Mouse)
		{
			if (event.GetCode() == VK_ESCAPE)
			{
				wnd->DisableCursor();
				state = Mode::Ghost;
				pActiveCamera = &camera;
			}
			else
			{
				wnd->DisableCursor();
				state = Mode::Player;
				pActiveCamera = &player;
			}
		}
		else if (state == Mode::Ghost)
		{
			if (event.GetCode() == VK_ESCAPE)
			{
				wnd->EnableCursor();
				state = Mode::Mouse;
			}
			else
			{
				state = Mode::Player;
				pActiveCamera = &player;
			}
		}
		else if (state == Mode::Player)
		{
			if (event.GetCode() == VK_ESCAPE)
			{
				wnd->EnableCursor();
				state = Mode::Mouse;
			}
			else
			{
				state = Mode::Ghost;
				pActiveCamera = &camera;
			}
		}
	}
}

void TrynGameApp::ShowStateInfo()
{
	if (ImGui::Begin("State Info"))
	{
		switch (state)
		{
			case Mode::Ghost:
				ImGui::Text("Ghost");
				break;
			case Mode::Player:
				ImGui::Text("Player");
				break;
			case Mode::Mouse:
				ImGui::Text("Mouse");
				break;
		}
		ImGui::End();
	}
}
