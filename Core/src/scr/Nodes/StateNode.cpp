#include "TrynPCH.h"
#include "StateNode.h"
#include "Core/src/gph/PinInfo.h"
#include <Core/src/scr/ScriptGraph.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <Core/src/log/Log.h>

namespace tryn::scr
{
	StateNode::StateNode(ScriptGraph* editorApp, const std::string& name, std::vector<std::string> states_):
		ScriptNode(editorApp, name), numStates((uint16_t)states.size()), states(std::move(states_))
	{
		// Input Pin
		{
			gph::PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = gph::PinKind::Input, .id = editorApp->uniqueId++ };
			editorApp->pinIdToInfo.insert({info.id, info});
			pinIds.push_back(info.id);
		}

		// Output Pins
		for (const auto& state : states)
		{
			gph::PinInfo info{ .parentId = this->uniqueId, .name = "Out " + state, .kind = gph::PinKind::Output, .id = editorApp->uniqueId++ };
			editorApp->pinIdToInfo.insert({info.id, info});
			pinIds.push_back(info.id);
		}
	}

	void StateNode::ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour)
	{
		if(ImGui::Begin("StateNodeCreate"))
		{
			static std::string name;
			if (ImGui::InputText("Node Name", &name)) {}

			static auto stateCount = 1;
			if (ImGui::InputInt("Number of States", &stateCount, 1, 3))
			{
				if (stateCount <= 0)
				{
					trylog.warn(L"The state count cannot be 0 or negative");
					stateCount = 1;
				}
				if (stateCount >= 50)
				{
					trylog.warn(L"The state count cannot exceed 50");
					stateCount = 49;
				}
			}
			static std::vector<std::string> states;
			states.resize(stateCount);
			ImGui::Text("States:");
			for (int i = 0; i < stateCount; i++)
			{
				ImGui::InputText(std::to_string(i).c_str(), &states[i]);
			}

			if (ImGui::Button("Create"))
			{
				graph->CreateNewNode(std::make_unique<StateNode>(graph, std::string{name}, std::vector<std::string>{states}), screenPos);
				name = {};
				states = {};
				finalBehaviour();
			}
		}
		ImGui::End();
	}

	unsigned long long StateNode::Execute()
	{
		const auto& info = pGraph->pinIdToInfo[pinIds[currentState]];
		return info.linked ? info.linkedId : uniqueId;
	}
}
