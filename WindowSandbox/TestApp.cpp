#include "TestApp.h"
#include <Core/src/app/EntryPoint.h>
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>
#include <utility>
#include <Core/src/gfx/Profiler.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <core/src/gfx/RenderQueue/Technique.h>
#include <Core/src/gfx/RenderQueue/Step.h>
#include <Core/src/gfx/Assimp.h>

tryn::app::App* tryn::app::CreateApp(int argc, char** argv)
{
	auto window = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{ .size = spa::DimensionsI{.width = (1280), .height = (720) } });
	window->SetTitle(L"Test WindowApp");

	auto gfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{ window->GetClientDimensions().width, window->GetClientDimensions().height, window->GetHandle() });

	app::App* app = new TestApp(window, gfx);

	return app;
}

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx)
{
	this->wnd = std::move(wnd);
	this->gfx = std::move(gfx);

	// Graphic Matrices
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().dimensions.width), static_cast<float>(Gfx().dimensions.height), 0.1f, 1000.0f));

	camera.GetPosition() = {0.0f,0.0f,-3.0f};

	//suzanne1 = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne1", "resources/models/greenSuzanne.fbx", glm::vec3{ 0.01f,0.01f,0.01f });
	//suzanne2 = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne2", "resources/models/suzanneHp.obj");
	//suzanne3 = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne3", "resources/models/suzanneHp.obj", glm::vec3{ 0.01f,0.01f,0.01f });
	//suzanne4 = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne4", "resources/models/suzanneHp.obj", glm::vec3{ 0.01f,0.01f,0.01f });
	//sponza = std::make_unique<ent::BasicEntity>(Gfx(), "sponza", "resources/models/Sponza/sponza.obj", glm::vec3{0.01f,0.01f,0.01f});
	gobber = std::make_unique<ent::BasicEntity>(Gfx(), "gobber", "resources/models/gobber/GoblinX.obj", glm::vec3{ 0.1f,0.1f,0.1f });

	pPointLight = std::make_unique<gfx::PointLight>(Gfx(), 0.01f);
	//wall = std::make_unique<ent::BasicEntity>(Gfx(), "wall", "resources/models/brick_wall/brick_wall.obj");
	this->wnd->keyboard.DisableAutoRepeat();
}

void TestApp::DoFrame()
{
	camera.Bind(Gfx());
	pPointLight->Bind(camera.GetViewMatrix());
	{
		static constexpr float angle = 0.001f;
		PROFILE_SCOPE("Update Rotation");

		pPointLight->ShowControls();
		pPointLight->GetModel().SpawnControlWindow();
	}
	{
		PROFILE_SCOPE("Draw call");
		pPointLight->Draw();
		//suzanne1->Draw();
		//suzanne2->Draw();
		gobber->Draw();
		//suzanne3->Draw();
		//suzanne4->Draw();
		//sponza->Draw();
		//wall->Draw();
	}
	{
		PROFILE_SCOPE("Update Camera");
		auto& cameraPos = camera.GetPosition();
		auto& cameraDirection = camera.GetDirection();

		constexpr auto upDirection = glm::vec3(0, 1.0f, 0);

		const auto newMousePos = wnd->mouse.GetPos();

		//Controls
		if (!wnd->IsCursorEnabled())
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
}
