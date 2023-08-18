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
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().dimensions.width), static_cast<float>(Gfx().dimensions.height), 0.1f, 100.0f));

	camera.GetPosition() = {0.0f,0.0f,-3.0f};

	suzanne = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne", "resources/models/suzanneHp.obj");
	oldBuilding = std::make_unique<ent::BasicEntity>(Gfx(), "old_building", "resources/models/Old_Building/Old_Building_obj.obj");
	pPointLight = std::make_unique<gfx::PointLight>(Gfx());
}

void TestApp::DoFrame()
{
	camera.Bind(Gfx());
	pPointLight->Bind();
	{
		static constexpr float angle = 0.001f;
		PROFILE_SCOPE("Update Rotation");
		//suzanne->GetModel().SpawnControlWindow();
		oldBuilding->GetModel().SpawnControlWindow();
		pPointLight->ShowControls();
		pPointLight->GetModel().SpawnControlWindow();
	}
	{
		PROFILE_SCOPE("Draw call");
		pPointLight->Draw();
 		//suzanne->Draw();
		oldBuilding->Draw();
	}
	{
		PROFILE_SCOPE("Update Camera");
		auto& cameraPos = camera.GetPosition();
		auto& cameraAngles = camera.GetAngles();
		if (wnd->keyboard.IsKeyPressed('A'))
		{
			cameraPos.x -= 0.3;
		}
		if (wnd->keyboard.IsKeyPressed('S'))
		{
			cameraPos.y -= 0.3;
		}
		if (wnd->keyboard.IsKeyPressed('D'))
		{
			cameraPos.x += 0.3;
		}
		if (wnd->keyboard.IsKeyPressed('W'))
		{
			cameraPos.y += 0.3;
		}
		if (wnd->keyboard.IsKeyPressed(VK_SPACE))
		{
			cameraPos.z += 0.3;
		}
		if (wnd->keyboard.IsKeyPressed(VK_MENU))
		{
			cameraPos.z -= 0.3;
		}
	}
	static bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);
	ImGui::Begin("Test", &showDemoWindow, 0);
	ImGui::End();
}