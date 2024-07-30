#include "App.h"
#include <Core/src/ecs/Entity.h>
#include <chrono>
#include <Core/src/gfx/Profiler.h>
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/win/IWindow.h>

namespace tryn::app
{
	App::App(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx)
		:
		wnd(wnd),
		gfx(gfx)
	{}
	void App::Go()
	{
		while (!wnd->IsClosing())
		{
		//	trylog.debug(L"Started Frame");
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
		ecs::sys::SystemManager::Get().ExecuteSystems();
		gfx->EndFrame();
	}

	const gfx::IGraphics& App::Gfx() const
	{
		return *gfx;
	}

	gfx::IGraphics& App::Gfx()
	{
		return *gfx;
	}
}
