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
	constexpr auto eyePos = glm::vec3(0, 0, -6);
	constexpr auto focusPoint = glm::vec3(0, 0, 0);
	constexpr auto upDirection = glm::vec3(0, 1, 0);
	Gfx().SetCamera(glm::lookAtLH(eyePos, focusPoint, upDirection));
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().dimensions.width), static_cast<float>(Gfx().dimensions.height), 0.1f, 100.0f));

	suzanne = std::make_unique<ent::BasicEntity>(Gfx(), "suzanne", "resources/models/suzanneHp.obj");
}

void TestApp::DoFrame()
{
	{
		static constexpr float angle = 0.001f;
		PROFILE_SCOPE("Update Rotation");
		suzanne->GetModel().SpawnControlWindow();
	}
	{
		PROFILE_SCOPE("Draw call");
 		suzanne->Draw();
	}
	//static bool showDemoWindow = true;
	//ImGui::ShowDemoWindow(&showDemoWindow);
	//ImGui::Begin("Test", &showDemoWindow, 0);
	//ImGui::End();
}