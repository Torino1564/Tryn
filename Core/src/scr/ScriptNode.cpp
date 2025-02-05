#include "TrynPCH.h"
#include "ScriptNode.h"
#include "ScriptGraph.h"

namespace tryn::scr
{
	void ScriptNode::Traverse(ScriptGraph& graph) const
	{
		for (auto& childId : childrenIds)
		{
			if (auto& child = *graph.nodes[childId]; child.Condition())
				graph.currentNodeId = childId;
		}
	}

	void ScriptNode::Run(ScriptGraph& graph) const
	{
		Traverse(graph);
	}
}
