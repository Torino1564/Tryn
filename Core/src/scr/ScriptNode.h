#pragma once
#include <Core/src/gph/TNode.h>
#include "ScriptGraph.h"

namespace tryn::scr
{
	class ScriptNode : public gph::TNode
	{
	public:
		ScriptNode(ScriptGraph* pGraph, std::string_view name, spa::Vec2I position = {});
		static void ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);

		ScriptGraph* pGraph = nullptr;
	};
}