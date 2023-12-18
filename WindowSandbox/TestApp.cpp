#include "TestApp.h"
#include <utility>
#include <ranges>
#include <Core/src/app/EntryPoint.h>
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>
#include <Core/src/gfx/Profiler.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <core/src/gfx/RenderQueue/Technique.h>
#include <Core/src/gfx/RenderQueue/Step.h>
#include <Core/src/gfx/Assimp.h>
#include <Core/src/ccr/Master.h>
#include <Core/src/ent/Component/ModelComponent.h>
#include <Core/src/ent/Component/PositionComponent.h>

class TestRenderGraph : public gfx::IRenderGraph
{
public:
	TestRenderGraph(gfx::IGraphics& gfx_p)
		:
		master(gbl::configs.numRenderWorkers)
	{
		gfx = &gfx_p;
		for (int i = 0; i < gbl::configs.numRenderWorkers; i++)
		{
			workerPtrs.push_back(gfx->CreateRenderWorker(&master));
			workerPtrs[i]->StartWorking();
		}
	}
	void ExecuteFrame(gfx::IGraphics& gfx) override
	{
		{
			PROFILE_SCOPE("Execute Frame");
			for (auto [selectedPointLight, pointLight] : std::views::enumerate(pPointLights))
			{
				pointLight->Bind();

				if (gbl::configs.singleThreadedRendeder)
				{
					GetRenderQueueByID("Lambertian").RunJobs(gfx);
				}
				else
				{
					GetRenderQueueByID("Lambertian").RunJobsAsync(gfx, master, workerPtrs, pPointLights[selectedPointLight]);
				}

			}
		}
	}
private:
	ccr::Master master;
	std::vector<std::unique_ptr<gfx::RenderWorker>> workerPtrs;
};

tryn::app::App* tryn::app::CreateApp(int argc, char** argv)
{
	auto window = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{ .size = spa::DimensionsI{.width = (1280), .height = (720) } });
	window->SetTitle(L"Test WindowApp");

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

	app::App* app = new TestApp(window, gfx);

	return app;
}

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx)
{
	this->wnd = std::move(wnd);
	this->gfx = std::move(gfx);

	// Graphic Matrices
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().dimensions.width), static_cast<float>(Gfx().dimensions.height), 0.1f, 10000000000.0f));
	Gfx().SetRenderGraph(std::make_unique<TestRenderGraph>(Gfx()));

	camera.GetPosition() = {0.0f,0.0f,-3.0f};

	for (int i = 0; i < pow(entityCount1D, 3); i++)
	{
		//entities.emplace_back("gobber" + std::to_string(i));
		//auto& srd = entities.back().AddComponent<ent::cmp::ModelComponent>();
		//srd = ent::cmp::ModelComponent::Construct(Gfx(), "resources/models/gobber/GoblinX.obj");
	}

	for (int i = 0; i < entityCount1D; i++)
	{
		for (int j = 0; j < entityCount1D; j++)
		{
			for (int k = 0; k < entityCount1D; k++)
			{
				/*auto pPos = entities[i + (entityCount1D * j) + (entityCount1D * entityCount1D * k)].GetComponent<ent::cmp::PositionComponent>();
				if (pPos != nullptr)
				{
					pPos->position.x = -entityCount1D / 2. + i;
					pPos->position.y = -entityCount1D / 2. + j;
					pPos->position.z = -entityCount1D / 2. + k;
				}*/
			}
		}
	}
	//entities.emplace_back(std::make_unique<ent::BasicEntity>(Gfx(), "testPlane", "resources/models/TestPlane.fbx"));
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
		for (auto& entity : entities)
		{
			entity.SpawnControlWindow();
		}
	}
	{
		PROFILE_SCOPE("Draw call");
		pPointLight->Submit(Gfx(),camera.GetViewMatrix());
		for (auto& entity : entities)
		{
			entity.MarkForUpdate();
		}
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
	
}
