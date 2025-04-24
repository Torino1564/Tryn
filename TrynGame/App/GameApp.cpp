#include "GameApp.h"

#include <imgui.h>
#include <Engine/Render/RenderGraph.h>
#include <TrynGame/Game/Core/Player.h>
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/win/IWindow.h>

#include "Core/src/ecs/sys/SystemManager.h"

TrynGameApp::TrynGameApp(std::shared_ptr<tryn::win::IWindow> pWindow, std::shared_ptr<tryn::gfx::IGraphics> pGraphics)
	: App(pWindow, pGraphics)
{
	Gfx().SetRenderGraph(std::make_unique<TrynGameRenderGraph>(Gfx()));
	ECS().GetSystemManager().Finalize();
	Gfx().SetBackgroundColor(000.0f, 000.0f, 000.0f, 1);
	wnd->keyboard.DisableAutoRepeat();

	{
		pPlayer = std::make_unique<Player>(ECS(), "player1", "Game/Resources/Models/PlayerModels/ShinySphere/ShinySphere.obj", Gfx());

		pPlayer->GetComponent<ecs::PositionComponent>().position = {10.0f, 10.0f, 10.0f};
	}

	{
		entities.emplace_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		   ecs::ActiveComponent,
		   ecs::PositionComponent,
		   ecs::TransformComponent,
		   ecs::ModelComponent,
		   ecs::ScaleComponent,
		   ecs::RotationComponent,
		   ecs::PointLightComponent>(ECS(), "light")));

		const auto& pLight = entities.back();

		pLight->GetComponent<ecs::PositionComponent>().position = { 0.0f, 20.0f, 0.0f };
		pLight->GetComponent<ecs::ModelComponent>().pModel = gfx::Model::Make<gfx::Flat>(Gfx(), "Game/Resources/Models/sphere.obj");
		pLight->GetComponent<ecs::ActiveComponent>().active = true;
		pLight->GetComponent<ecs::PointLightComponent>().parameters = gfx::PointLightParameters{
			.ambient = {0.1f, 0.1f, 0.1f},
			.diffuseColor = glm::normalize(glm::vec3{1.0f, 1.0f, 1.0f}),
			.diffuseIntensity = 1.0f,
			.constantAtt = 1.0f,
			.linearAtt = 0.045f,
			.quadraticAtt = 0.0075f
		};
		pLight->GetComponent<ecs::ScaleComponent>().scale = {1.0f, 1.0f, 1.0f};
	}

	//{
	//	entities.emplace_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
	//	   ecs::ActiveComponent,
	//	   ecs::PositionComponent,
	//	   ecs::TransformComponent,
	//	   ecs::ModelComponent,
	//	   ecs::ScaleComponent,
	//	   ecs::RotationComponent>(ECS(),"sponza")));

	//	auto& sponza = *entities.back();

	//	sponza.GetComponent<ecs::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
	//	sponza.GetComponent<ecs::ModelComponent>().pModel = std::make_unique<gfx::Model>(Gfx(), "Game/Resources/Models/Sponza/sponza.obj");
	//	sponza.GetComponent<ecs::ScaleComponent>().scale = { 0.01f, 0.01f, 0.01f };
	//	sponza.GetComponent<ecs::ActiveComponent>().active = true;
	//}

	{
		entities.push_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		   ecs::ActiveComponent,
		   ecs::PositionComponent,
		   ecs::TransformComponent,
		   ecs::ModelComponent,
		   ecs::ScaleComponent,
		   ecs::RotationComponent>(ECS(),"TestPlane")));

		auto& plane = *entities.back();

		plane.GetComponent<ecs::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
		plane.GetComponent<ecs::ModelComponent>().pModel = std::make_unique<gfx::Model>(Gfx(), "Game/Resources/Models/Environments/TestPlane.obj");
		plane.GetComponent<ecs::ScaleComponent>().scale = { 10.0f, 10.0f, 10.0f };
		plane.GetComponent<ecs::ActiveComponent>().active = true;
	}

	{
		entities.push_back(std::make_unique<ecs::Entity>(ecs::Entity::CreateNew<
		   ecs::ActiveComponent,
		   ecs::PositionComponent,
		   ecs::TransformComponent,
		   ecs::ModelComponent,
		   ecs::ScaleComponent,
		   ecs::AnimatedComponent,
		   ecs::BoneTransformsComponent,
		   ecs::RotationComponent>(ECS(),"AnimationTest")));

		auto& ent = *entities.back();

		ent.GetComponent<ecs::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
		ent.GetComponent<ecs::ModelComponent>().pModel = std::unique_ptr<gfx::Model>( new gfx::Model(Gfx(), "Game/Resources/Models/adamHead/adamHead.gltf", std::array{ZT_TYPE_UUID(gfx::ForwardPhong)}));
		//ent.GetComponent<ecs::AnimatedComponent>().pAnimationSkeletonInterface = ent.GetComponent<ecs::ModelComponent>().pModel->GetMainMesh()->GetAnimationInterface();
		ent.GetComponent<ecs::ScaleComponent>().scale = { 1.0f, 1.0f, 1.0f };
		ent.GetComponent<ecs::ActiveComponent>().active = true;
	}

	{
		camera.GetPosition() = { 0.0f, 10.0f, 0.0f };
		camera.GetDirection() = { 0.0f, 0.0f, 0.0f };

		player.SetPitch(75.0f);
		player.SetYaw(45.0f);
		player.GetPosition() = { 0.0f, 22.5f, 0.0f };
	}


}

void TrynGameApp::DoFrame()
{
	// Process input
	auto& playerVelocity = pPlayer->GetComponent<ecs::VelocityComponent>().velocity;
	playerVelocity = { 0.0f, 0.0f, 0.0f };
	pActiveCamera->Submit(Gfx());

	for (const auto& pEntity : entities)
	{
		pEntity->SpawnControlWindow();
	}
	
	if (state == Mode::Player)
	{
		auto& playerCameraPosition = player.GetPosition();
		auto& playerObjectPosition = pPlayer->GetComponent<ecs::PositionComponent>().position;
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
	}
	ImGui::End();
}
