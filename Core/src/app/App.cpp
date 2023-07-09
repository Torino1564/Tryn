#include "App.h"
#include <Core/src/ent/Entity.h>

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
        gfx->BeginFrame();
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