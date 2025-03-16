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
    	if (ImGui::Begin("MainWindow", &flag, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus))
	    {
		    if (ImGui::BeginMainMenuBar())
		    {
		    	if (ImGui::BeginMenu("File"))
		    	{

					 
		    		ImGui::EndMenu();
		    	}
		    	if (ImGui::BeginMenu("Edit"))
		    	{


		    		ImGui::EndMenu();
		    	}
		    	if (ImGui::BeginMenu("Tools"))
		    	{
		    		for (auto [uuid, typeInfo] : appletRegister.infoTable)
		    		{
		    			if (ImGui::MenuItem(typeInfo.name))
		    			{
		    				CreateApplet(uuid);
		    			}
		    		}

		    		ImGui::EndMenu();
		    	}
		    	ImGui::EndMainMenuBar();
		    }

        	ImGui::ShowDemoWindow();

        	for (auto& pApplet : pApplets)
        	{
        		pApplet->DoFrame();
        	}
	    }

        ImGui::End();
    }

    void TrynEditorApp::CreateApplet(utl::UUID_t uuid)
    {
		Applet* pNewApplet = nullptr;
    	appletRegister.ConstructAndFill(&pNewApplet, uuid);
		pNewApplet->instanceID = appletIdCounter++;
    	pApplets.emplace_back(pNewApplet);
    }
}

