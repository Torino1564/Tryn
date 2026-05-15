
#include "TGraph.h"
#include "TNode.h"

namespace tryn::gph
{
    TGraph::~TGraph()
    {
        links.clear();
        nodes.clear();

        nodeIdToNodeIndex.clear();
        pinIdToInfo.clear();
    }

    TGraph::TGraph(const std::string_view name) : name(name) {}

    TNode& TGraph::CreateNewNode(std::unique_ptr<TNode>&& newVal, const std::optional<spa::Vec2I> pos)
	{
    	nodes.push_back(std::forward<std::unique_ptr<TNode>>(newVal));
        nodeIdToNodeIndex.insert({nodes.back()->uniqueId, static_cast<uint16_t>(nodes.size() - 1)});

        if (pos.has_value())
        {
	        nodes.back()->position = pos.value();
        }

        return *nodes.back();
	}

    void TGraph::CreateNewLink(unsigned long long linkId, PinInfo& pin1, PinInfo& pin2)
	{
		links.push_back(std::move(std::make_unique<TLink>(linkId, pin1, pin2, this)));

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

    void TGraph::CreateNewLink(std::unique_ptr<TLink>&& newVal)
    {
        links.push_back(std::move(newVal));
    }

    void TGraph::ExecuteStep()
    {
        const auto currentNodeIndex = this->nodeIdToNodeIndex[currentNodeId];

        currentNodeId = nodes[currentNodeIndex]->Execute();
    }
}