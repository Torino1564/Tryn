#include "App.h"
#include <Core/src/ent/Entity.h>
#include <Core/src/win/imgui_impl_win32.h>

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
            PreFrame();
            DoFrame();
            PostFrame();
        }
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
        ImGui::EndFrame();
        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        gfx->EndFrame();
    }

    gfx::IGraphics& App::Gfx()
    {
        return *gfx;
    }

}