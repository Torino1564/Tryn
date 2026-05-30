
#include "EntryNode.h"

#include <imgui.h>
#include <Core/src/gph/PinInfo.h>
#include <Core/src/scr/ScriptGraph.h>
#include "Core/src/utl/Assert.h"
#include <imgui_stdlib.h>

namespace tryn::scr
{
	EntryNode::EntryNode(ScriptGraph* pGraph, const std::string& name): ScriptNode (pGraph, name)
	{
		// Output Pins
		{
			gph::PinInfo info{ .parentId = this->uniqueId, .name = "Out", .kind = gph::PinKind::Output, .id = pGraph->uniqueId++ };
			pGraph->pinIdToInfo.insert({ info.id, info });
			pinIds.push_back(info.id);
		}

		trynass(!pGraph->entryId.has_value()).msg(L"The script already has an Entry Node!");

		pGraph->entryId.emplace(uniqueId);
	}

	unsigned long long EntryNode::Execute()
	{
		const auto& info = pGraph->pinIdToInfo[pinIds[0]];
		return info.linked ? info.linkedId : uniqueId;
	}

	void EntryNode::ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour)
	{
		if(ImGui::Begin("EntryNodeCreate"))
		{
			static std::string name;
			if (ImGui::InputText("Node Name", &name)) {}

			if (ImGui::Button("Create"))
			{
				graph->CreateNewNode(std::move(std::make_unique<EntryNode>(graph, std::string{name})), screenPos);
				finalBehaviour();
				name = {};
			}
		}
		ImGui::End();
	}
}
