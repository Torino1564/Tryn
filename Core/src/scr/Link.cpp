#include "TrynPCH.h"
#include "Link.h"
#include <Core/src/scr/ScriptGraph.h>
#include <Core/src/scr/ScriptNode.h>
#include <Core/src/ser/StreamIO.h>

namespace tryn::scr
{
	Link& Link::operator=(Link&& rhs) noexcept
    {
        Link temp(std::move(rhs));

        std::swap(Id, temp.Id);
        std::swap(InputId, temp.InputId);
        std::swap(OutputId, temp.OutputId);
        std::swap(pGraph, temp.pGraph);

        return *this;
    }

	Link::Link(Link&& rhs) noexcept
	{
		Id = rhs.Id;
		InputId = rhs.InputId;
		OutputId = rhs.OutputId;
		pGraph = std::exchange(rhs.pGraph, nullptr);

	}

	Link::Link(const unsigned long long id, PinInfo& pin1, PinInfo& pin2, ScriptGraph* pGraph)
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

	Link::~Link()
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
	void SerializeWrite(const StreamWriter& sw, const scr::Link& data, const bool binary,
	   const std::string& name)
	{
		sw.Serialize(data.Id, binary);
		sw.Serialize(data.InputId, binary);
		sw.Serialize(data.OutputId, binary);
	}

	void SerializeRead(const StreamReader& sr, scr::Link& data_, const bool binary, ExtraDataPack* pExtraData)
	{
		pExtraData->Get("pGraph").Get((void*&)data_.pGraph);
		sr.ReadSerialized(data_.Id, binary);
		sr.ReadSerialized(data_.InputId, binary);
		sr.ReadSerialized(data_.OutputId, binary);
	}
}
