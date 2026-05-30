#pragma once
#include <Core/src/scr/ScriptNode.h>

namespace tryn::scr
{
	 struct StateNode : public ScriptNode
    {
        StateNode() = default;
        StateNode(ScriptGraph* editorApp, const std::string& name, std::vector<std::string> states_);
        static void ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);
        unsigned long long Execute() override;

    private:
        uint16_t numStates = 0;
        uint16_t currentState = 1;
        std::vector<std::string> states;
    };

}
