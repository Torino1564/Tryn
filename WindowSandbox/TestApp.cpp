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
#include <core/src/gfx/Render/Technique.h>
#include <Core/src/gfx/Render/Step.h>
#include <Core/src/gfx/Assimp.h>
#include <Core/src/ccr/Master.h>
#include <Core/src/ecs/cmp/ActiveComponent.h>
#include <Core/src/ecs/cmp/ModelComponent.h>
#include <Core/src/ecs/cmp/ScaleComponent.h>
#include <Core/src/ecs/cmp/RotationComponent.h>
#include <Core/src/ecs/cmp/PositionComponent.h>
#include <Core/src/ecs/cmp/VelocityComponent.h>
#include <Core/src/ecs/cmp/AccelerationComponent.h>
#include <Core/src/ecs/cmp/AnimatedComponent.h>
#include <Core/src/ecs/cmp/BoneTransformsComponents.h>
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/ecs/sys/TransformSystem.h>
#include <Core/src/ecs/sys/RenderSystem.h>
#include <Core/src/ecs/sys/UpdatePositionSystem.h>
#include <Core/src/ecs/sys/UpdateVelocitySystem.h>
#include <Core/src/ecs/sys/AnimationSystem.h>
#include <Core/src/gfx/Animation/AnimationManager.h>

class TestRenderGraph : public gfx::IRenderGraph
{
public:
	TestRenderGraph(gfx::IGraphics& gfx)
		:
		gfx::IRenderGraph(gfx), master(gbl::configs.numRenderWorkers)
	{
		for (int i = 0; i < gbl::configs.numRenderWorkers; i++)
		{
			workerPtrs.push_back(gfx.CreateRenderWorker(&master));
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
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().GetDimensions().width), static_cast<float>(Gfx().GetDimensions().height), 0.1f, 10000000000.0f));
	Gfx().SetRenderGraph(std::make_unique<TestRenderGraph>(Gfx()));

	camera.GetPosition() = {0.0f,0.0f,-3.0f};

	auto& sysManager = ecs::sys::SystemManager::Get();
	sysManager.RegisterSystem<ecs::sys::TransformSystem>();
	sysManager.RegisterSystem<ecs::sys::RenderSystem>();
	sysManager.RegisterSystem<ecs::sys::UpdatePositionSystem>();
	sysManager.RegisterSystem<ecs::sys::UpdateVelocitySystem>();
	sysManager.RegisterSystem<ecs::sys::AnimationSystem>();
	sysManager.Finalize();

	auto entParent = ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::InstancedModelParentComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::VelocityComponent,
		ecs::cmp::AccelerationComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent>("GobberParent");

	auto entChild = ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::InstancedModelChildComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::VelocityComponent,
		ecs::cmp::AccelerationComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent>("GobberChild");

	entities.resize(pow(entityCount1D, 3));
	entChild.Instanciate({ entities });

	/*auto mutant = ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::AnimatedComponent,
		ecs::cmp::BoneTransformsComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent
	>("Mutant");

	mutant.GetComponent<ecs::cmp::ActiveComponent>().active = true;
	mutant.GetComponent<ecs::cmp::ScaleComponent>().scale = { 0.05f,0.05f,0.05f };
	mutant.GetComponent<ecs::cmp::PositionComponent>().position = { 0.0f, 5.0f, 0.0f };
	mutant.GetComponent<ecs::cmp::ModelComponent>().pModel = std::make_unique<gfx::Model>(Gfx(), "resources/models/Mutant.fbx");

	auto animID = gfx::ani::AnimationManager::Get().New("resources/animations/Capoeira.fbx");

	auto mainMesh = mutant.GetComponent<ecs::cmp::ModelComponent>().pModel->GetMainMesh();
	mainMesh->AddAnimation(gfx::ani::AnimationManager::Get().Resolve(animID), "capoeira");
	auto& mutantAnimatedCmp = mutant.GetComponent<ecs::cmp::AnimatedComponent>();
	mutantAnimatedCmp.previousKey = 0;
	mutantAnimatedCmp.pAnimationSkeletonInterface = mainMesh->GetAnimationInterface("capoeira");
	mutantAnimatedCmp.state = gfx::ani::AnimationState::Playing;
	mutantAnimatedCmp.time = 0;*/

	entParent.GetComponent<ecs::cmp::InstancedModelParentComponent>().parentModel = gfx::InstancedModelParent(Gfx(), "resources/models/gobber/GoblinX.obj");
	entParent.GetComponent<ecs::cmp::ActiveComponent>().active = true;
	entParent.GetComponent<ecs::cmp::ScaleComponent>().scale = { .3f,.3f,.3f };
	entParent.GetComponent<ecs::cmp::VelocityComponent>().velocity = { .0f, 0.f, 0.f };
	entParent.GetComponent<ecs::cmp::AccelerationComponent>().acceleration = { .0f, 0.f, 0.f };

	auto& instanceParent = entParent.GetComponent<ecs::cmp::InstancedModelParentComponent>().parentModel;

	for (auto& entity : entities)
	{
		entity.GetComponent<ecs::cmp::InstancedModelChildComponent>().childModel = instanceParent.Instanciate();
		entity.GetComponent<ecs::cmp::ActiveComponent>().active = true;
		entity.GetComponent<ecs::cmp::ScaleComponent>().scale = { .3f,.3f,.3f };
		entity.GetComponent<ecs::cmp::VelocityComponent>().velocity = { .0f, 0.f, 0.f };
		entity.GetComponent<ecs::cmp::AccelerationComponent>().acceleration = { .0f, 0.f, 0.f };
	}

	for (int i = 0; i < entityCount1D; i++)
	{
		for (int j = 0; j < entityCount1D; j++)
		{
			for (int k = 0; k < entityCount1D; k++)
			{
				auto& pPos = entities[i + (entityCount1D * j) + (entityCount1D * entityCount1D * k)].GetComponent<ecs::cmp::PositionComponent>();
				pPos.position.x = -entityCount1D / 2. + i;
				pPos.position.y = -entityCount1D / 2. + j;
				pPos.position.z = -entityCount1D / 2. + k;
			}
		}
	}
	
	auto testTerrain = ecs::Entity::CreateNew<
		ecs::cmp::ActiveComponent,
		ecs::cmp::PositionComponent,
		ecs::cmp::TransformComponent,
		ecs::cmp::ModelComponent,
		ecs::cmp::ScaleComponent,
		ecs::cmp::RotationComponent>("testTerrain");

	testTerrain.GetComponent<ecs::cmp::PositionComponent>().position = { 0.0f, 0.0f, 0.0f };
	testTerrain.GetComponent<ecs::cmp::ModelComponent>().pModel = std::make_unique<gfx::Model>(Gfx(), "resources/models/TestTerrain.obj");
	testTerrain.GetComponent<ecs::cmp::ScaleComponent>().scale = { 1.0f, 1.0f, 1.0f };
	testTerrain.GetComponent<ecs::cmp::ActiveComponent>().active = true;

	entities.push_back(std::move(testTerrain));
	pPointLight = std::make_unique<gfx::PointLight>(Gfx(), 0.01f);

	this->wnd->keyboard.DisableAutoRepeat();
}

void TestApp::DoFrame()
{
	mem::ArenaAllocator<>::GP().Wipe();	
	camera.Submit(Gfx());
	pPointLight->SubmitLight(Gfx());

	{
		PROFILE_SCOPE("Update Rotation");
		pPointLight->ShowControls();
		for (auto& entity : entities)
		{
			//entity.SpawnControlWindow();
		}
	}
	{
		PROFILE_SCOPE("Draw call");
		pPointLight->Submit(Gfx(),camera.GetViewMatrix());
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
	ecs::sys::SystemManager::Get().ExecuteSystems();
}
