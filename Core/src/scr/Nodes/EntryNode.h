#pragma once
#include <Core/src/scr/ScriptNode.h>

namespace tryn::scr
{
	struct EntryNode : public ScriptNode
    {
        EntryNode() = default;
	    EntryNode(ScriptGraph* pGraph, const std::string& name);

        unsigned long long Execute() override;

        static void ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);
    };
}