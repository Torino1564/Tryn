#include "TestApp.h"
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

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx)
{
	this->wnd = std::move(wnd);
	this->gfx = std::move(gfx);

	// Graphic Matrices
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().dimensions.width), static_cast<float>(Gfx().dimensions.height), 0.1f, 1000.0f));

	camera.GetPosition() = {0.0f,0.0f,-3.0f};

	suzanne = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne", "resources/models/suzanne.obj");
	//sponza = std::make_unique<ent::BasicEntity>(Gfx(), "sponza", "resources/models/Sponza/sponza.obj", glm::vec3{0.01f,0.01f,0.01f});
	pPointLight = std::make_unique<gfx::PointLight>(Gfx(), 0.01f);
	wall = std::make_unique<ent::BasicEntity>(Gfx(), "wall", "resources/models/brick_wall/brick_wall.obj");
}

void TestApp::DoFrame()
{
	camera.Bind(Gfx());
	pPointLight->Bind(camera.GetViewMatrix());
	{
		static constexpr float angle = 0.001f;
		PROFILE_SCOPE("Update Rotation");
		suzanne->GetModel().SpawnControlWindow();
		//sponza->GetModel().SpawnControlWindow();
		wall->GetModel().SpawnControlWindow();
		pPointLight->ShowControls();
		pPointLight->GetModel().SpawnControlWindow();
	}
	{
		PROFILE_SCOPE("Draw call");
		pPointLight->Draw();
 		//suzanne->Draw();
		//sponza->Draw();
		wall->Draw();
	}
	{
		PROFILE_SCOPE("Update Camera");
		auto& cameraPos = camera.GetPosition();
		auto& cameraAngles = camera.GetAngles();
		auto& cameraDirection = camera.GetDirection();

		constexpr auto upDirection = glm::vec3(0, 1.0f, 0);

		const auto newMousePos = wnd->mouse.GetPos();

		if (wnd->keyboard.IsKeyPressed('A'))
		{
			cameraPos += 0.3f * glm::normalize(glm::cross(cameraDirection,upDirection));
		}
		if (wnd->keyboard.IsKeyPressed('S'))
		{
			cameraPos -= 0.3f * cameraDirection;
		}
		if (wnd->keyboard.IsKeyPressed('D'))
		{
			cameraPos -= 0.3f * glm::normalize(glm::cross(cameraDirection, upDirection));
		}
		if (wnd->keyboard.IsKeyPressed('W'))
		{
			cameraPos += 0.3f * cameraDirection;
		}
		if (wnd->keyboard.IsKeyPressed(VK_SPACE))
		{
			cameraPos.y += 0.3;
		}
		if (wnd->keyboard.IsKeyPressed(VK_MENU))
		{
			cameraPos.y -= 0.3;
		}
		if (wnd->keyboard.IsKeyPressed(VK_UP))
		{
			cameraAngles.y = std::min(90.0f, cameraAngles.y + 2);
		}
		if (wnd->keyboard.IsKeyPressed(VK_DOWN))
		{
			cameraAngles.y = std::max(-90.0f, cameraAngles.y - 2);
		}
		if (wnd->keyboard.IsKeyPressed(VK_LEFT))
		{
			cameraAngles.x += 2;
		}
		if (wnd->keyboard.IsKeyPressed(VK_RIGHT))
		{
			cameraAngles.x -= 2;
		}
		/* {
			if (newMousePos.first > mousepos.x)
			{
				cameraAngles.x -= 2;
			}
			if (newMousePos.first < mousepos.x)
			{
				cameraAngles.x += 2;
			}
			if (newMousePos.second > mousepos.y)
			{
				cameraAngles.y += 2;
			}
			if (newMousePos.second < mousepos.y)
			{
				cameraAngles.y -= 2;
			}
		}*/

		mousepos.x = newMousePos.first;
		mousepos.y = newMousePos.second;
		
		camera.Update();
	}
	static bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);
	ImGui::Begin("Test", &showDemoWindow, 0);
	ImGui::End();
}