#include "TrynPCH.h"
#include "TNode.h"
#include "TGraph.h"

#include <Core/src/ser/StreamIO.h>
#include <nlohmann/json.hpp>

namespace tryn
{
	namespace gph
	{
		void TNode::Submit(const std::function<void(TNode&)>& submitBehaviour)
		{
			submitBehaviour(*this);
		}

		void TNode::ImGuiCreate(TGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour)
		{
		}

		TNode::TNode(TGraph* pGraph, const std::string_view name, spa::Vec2I position): uniqueId(pGraph->uniqueId++), name(name), pGraph(pGraph), position(position)
		{}
	}

	namespace ser
	{
		void Serialize(StreamIO& io, gph::TNode* data, const bool binary, const std::string& name)
		{
			io.GetAndFill("pGraph", reinterpret_cast<void*&>(data->pGraph));
			io.Field(&data->uniqueId, binary);
			io.Field(&data->pinIds, binary);
			io.Field(&data->childrenIds, binary);
			io.Field(&data->name, binary);
			io.Field(&data->position, binary);
		}
	}
}

