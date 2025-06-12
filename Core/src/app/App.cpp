#include "TrynPCH.h"
#include "App.h"
#include <Core/src/gfx/Profiler.h>
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/win/IWindow.h>

#include "Core/src/ecs/cmp/ComponentManager.h"

namespace tryn::app
{
	App::App(const std::shared_ptr<win::IWindow>& wnd, const std::shared_ptr<gfx::IGraphics>& gfx)
		:
		pEcs(std::make_unique<ecs::ECS>(this)),
		wnd(wnd),
		gfx(gfx)
	{
		/*pEcs->GetComponentManager().RegisterSingleton(gfx);
		pEcs->GetComponentManager().RegisterSingleton(wnd);
		pEcs->GetComponentManager().RegisterSingleton(std::shared_ptr<App>(this));*/
	}

	App::~App()
	{
	}

	void App::Go()
	{
		while (!wnd->IsClosing())
		{
			auto start = std::chrono::high_resolution_clock::now();
			PreFrame();
			DoFrame();
			PostFrame();
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = end - start;
			const auto ticks = (end - start).count();
			dt = static_cast<long double>(ticks / 1e6);
		}
	}

	void App::PreFrame()
	{
		wnd->NewFrame();
		gfx->BeginFrame();
		ImGui::NewFrame();
	}

	void App::DoFrame()
	{
	}

	void App::PostFrame()
	{
		pEcs->WipeAllocator();
		pEcs->ExecuteSystems();
		gfx->EndFrame();
		if (wnd->HasSizeChanged())
		{
			gfx->Resize();
			wnd->AcknowledgeSizeChange();
		}
		mem::ArenaAllocator<>::GP().Wipe();
	}

	const gfx::IGraphics& App::Gfx() const
	{
		return *gfx;
	}

	gfx::IGraphics& App::Gfx()
	{
		return *gfx;
	}

	ecs::ECS& App::ECS()
	{
		return *pEcs;
	}

	const ecs::ECS& App::ECS() const
	{
		return *pEcs;
	}

	void App::Resize(spa::DimensionsI newDimensions) const
	{
		auto future = wnd->Resize(newDimensions);
		gfx->Resize();
		future.get();
	}
}
