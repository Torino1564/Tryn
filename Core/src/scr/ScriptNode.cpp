#include "TrynPCH.h"
#include "ScriptNode.h"

namespace tryn::scr
{
	ScriptNode::ScriptNode(ScriptGraph* pGraph, const std::string_view name, const spa::Vec2I position):
		TNode(pGraph, name, position)
	{
		this->pGraph = pGraph;
	}

	void ScriptNode::ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour)
	{
	}
}
