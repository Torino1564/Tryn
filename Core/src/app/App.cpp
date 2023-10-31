#include "App.h"
#include <Core/src/ent/Entity.h>
#include <Core/src/win/imgui_impl_win32.h>
#include <chrono>
#include <Core/src/gfx/Profiler.h>

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
			auto start = std::chrono::high_resolution_clock::now();
			PreFrame();
			DoFrame();
			PostFrame();
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = end - start;
			dt = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		}
		trylog.info(L"Closing window");
	}

	void App::PreFrame()
	{
		ImGui_ImplWin32_NewFrame();
		gfx->BeginFrame();
		ImGui::NewFrame();
	}

	void App::DoFrame()
	{
	}

	void App::PostFrame()
	{
		gfx->EndFrame();
	}

	gfx::IGraphics& App::Gfx()
	{
		return *gfx;
	}
}