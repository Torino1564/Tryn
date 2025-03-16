#include "Applet.h"

#include <imgui.h>

void tryn::ed::Applet::DoFrame()
{
	ImGui::Begin((name + "##" + std::to_string(instanceID)).c_str());

	Kernel();

	ImGui::End();
}
