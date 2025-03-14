#include "EditorApp.h"
#include <TrynEditor/src/App/ScriptEditor/ScriptEditor.h>

namespace tryn::ed
{
    TrynEditorApp::TrynEditorApp(const std::shared_ptr<win::IWindow>& pWnd, const std::shared_ptr<gfx::IGraphics>& pGfx)
        : App(pWnd, pGfx)
    {
        ECS().GetSystemManager().Finalize();
        Resize({1920, 1080});
        wnd->SetResizableFlag(true);
        gfx->SetVsyncFlag(true);

        RegisterApplet<ScriptEditor>();
    }

    void TrynEditorApp::DoFrame()
	{
        ImGui::SetNextWindowSize({ (float)gfx->GetDimensions().width - 1, (float)gfx->GetDimensions().height - 1 });
        ImGui::SetNextWindowPos({0.0f, 0.0f}, 0);

        static bool flag = true;
        ImGui::Begin("MainWindow", &flag, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav);

        auto& io = ImGui::GetIO();

        ImGui::ShowDemoWindow();

        ImGui::End();
    }
}

