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

class TestRenderGraph : public gfx::IRenderGraph
{
public:
	TestRenderGraph()
	{
	}
	void ExecuteFrame(gfx::IGraphics& gfx) override
	{
		{
			PROFILE_SCOPE("Execute Frame");
			for (auto pointLight : pPointLights)
			{
				pointLight->Bind(pCameras[selectedCamera]->GetViewMatrix());
				GetRenderQueueByID("Lambertian").RunJobs(gfx);
			}
		}
	}
};

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
	Gfx().SetRenderGraph(std::make_unique<TestRenderGraph>());

	camera.GetPosition() = {0.0f,0.0f,-3.0f};

	gobber = std::make_unique<ent::BasicEntity>(Gfx(), "gobber", "resources/models/gobber/GoblinX.obj");
	//sponza = std::make_unique<ent::BasicEntity>(Gfx(), "sponza", "resources/models/Sponza/sponza.obj", glm::vec3{ 0.01f,0.01f,0.01f });

	pPointLight = std::make_unique<gfx::PointLight>(Gfx(), 0.01f);

	this->wnd->keyboard.DisableAutoRepeat();
}

void TestApp::DoFrame()
{
	
	camera.Submit(Gfx());
	pPointLight->SubmitLight(Gfx());

	{
		PROFILE_SCOPE("Update Rotation");
		pPointLight->ShowControls();
		gobber->SpawnControlWindow();
	}
	{
		PROFILE_SCOPE("Draw call");
		pPointLight->Submit(Gfx());
		gobber->Submit();
		//sponza->Submit();
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
