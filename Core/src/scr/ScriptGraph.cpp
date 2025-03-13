#include "TrynPCH.h"
#include "ScriptGraph.h"

#include <dylib.hpp>

#include "ScriptNode.h"
#include "PinInfo.h"

namespace tryn::scr
{
	ScriptNode& ScriptGraph::CreateNewNode(std::unique_ptr<ScriptNode>&& newVal, const std::optional<spa::Vec2I> pos)
	{
    	nodes.push_back(std::forward<std::unique_ptr<ScriptNode>>(newVal));
        nodeIdToNodeIndex.insert({nodes.back()->uniqueId, static_cast<uint16_t>(nodes.size() - 1)});

        if (pos.has_value())
        {
	        nodes.back()->position = pos.value();
        }

        return *nodes.back();
	}

    void ScriptGraph::CreateNewLink(unsigned long long linkId, PinInfo& pin1, PinInfo& pin2)
		{
			m_Links.emplace_back(linkId, pin1, pin2, this);

		    if (pin1.kind == PinKind::Input)
		    {
		        auto& outputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];
    			auto& inputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];

    			outputNode.childrenIds.push_back(nodeIdToNodeIndex[pin1.parentId]);
		    }
		    else
		    {
		        auto& outputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];
    			auto& inputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];

    			outputNode.childrenIds.push_back(nodeIdToNodeIndex[pin1.parentId]);
		    }
		}

    void ScriptGraph::ExecuteStep()
    {
        const auto currentNodeIndex = this->nodeIdToNodeIndex[currentNodeId];

        currentNodeId = nodes[currentNodeIndex]->Execute();
    }
}
