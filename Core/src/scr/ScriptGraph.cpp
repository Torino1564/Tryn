#include "TrynPCH.h"
#include "ScriptGraph.h"

#include "ScriptNode.h"

namespace tryn::scr
{
	void ScriptGraph::FillVariables(VariableManager* vManager) const
	{
		for (const auto& pNode : nodes)
		{
			pNode->RegisterVariables(vManager);
		}
	}
}
