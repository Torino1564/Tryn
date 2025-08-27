
#include "TLink.h"
#include "TGraph.h"
#include <Core/src/gph/TNode.h>
#include <Core/src/ser/StreamIO.h>

namespace tryn::gph
{
	TLink& TLink::operator=(TLink&& rhs) noexcept
    {
        TLink temp(std::move(rhs));

        std::swap(Id, temp.Id);
        std::swap(InputId, temp.InputId);
        std::swap(OutputId, temp.OutputId);
        std::swap(pGraph, temp.pGraph);

        return *this;
    }

	TLink::TLink(TLink&& rhs) noexcept
	{
		Id = rhs.Id;
		InputId = rhs.InputId;
		OutputId = rhs.OutputId;
		pGraph = std::exchange(rhs.pGraph, nullptr);

	}

	TLink::TLink(const unsigned long long id, PinInfo& pin1, PinInfo& pin2, TGraph* pGraph)
        :
        Id(id), pGraph(pGraph)
        {
            auto& nodes = pGraph->nodes;
            auto& nodeIdToNodeIndex = pGraph->nodeIdToNodeIndex;

		    if (pin1.kind == PinKind::Input)
		    {
		        auto& outputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];
    			auto& inputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];

    			outputNode.childrenIds.push_back(inputNode.uniqueId);

                InputId = pin1.id;
                OutputId = pin2.id;
		    }
		    else
		    {
		        auto& outputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];
    			auto& inputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];

    			outputNode.childrenIds.push_back(inputNode.uniqueId);

                InputId = pin2.id;
                OutputId = pin1.id;
		    }
            pin1.linked = true;
			pin2.linked = true;
			pin1.linkedId = pin2.parentId;
			pin2.linkedId = pin1.parentId;
        }

	TLink::~TLink()
    {
        if (!pGraph)
            return;

    	auto& inputPinInfo = pGraph->pinIdToInfo[InputId];
        auto& outputPinInfo = pGraph->pinIdToInfo[OutputId];

        auto& children = pGraph->nodes[pGraph->nodeIdToNodeIndex[outputPinInfo.parentId]]->childrenIds;

        for (auto it = children.begin(); it < children.end(); it++)
        {
	        if (*it == inputPinInfo.parentId)
	        {
		        children.erase(it);
                break;
	        }
        }

        inputPinInfo.linked = false;
        outputPinInfo.linked = false;
    }
}

namespace tryn::ser
{
	void Serialize(StreamIO& io, gph::TLink* data, const bool binary, const std::string& name)
	{
		io.GetAndFill("pGraph", reinterpret_cast<void*&>(data->pGraph));
		io.Field(&data->Id, binary);
		io.Field(&data->InputId, binary);
		io.Field(&data->OutputId, binary);
	}

}
