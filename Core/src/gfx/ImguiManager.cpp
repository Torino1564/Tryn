#include "ImguiManager.h"

namespace tryn::gfx
{
	ImguiManager::ImguiManager()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}
	ImguiManager::~ImguiManager()
	{
		ImGui::DestroyContext();
	}
}

