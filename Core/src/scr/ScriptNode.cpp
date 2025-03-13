#include "TrynPCH.h"
#include "ScriptNode.h"
#include "ScriptGraph.h"

#include <Core/src/ser/StreamIO.h>

namespace tryn
{
	namespace scr
	{
		void ScriptNode::Submit(const std::function<void(ScriptNode&)>& submitBehaviour)
		{
			submitBehaviour(*this);
		}

		void ScriptNode::ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour)
		{
		}

		ScriptNode::ScriptNode(ScriptGraph* pGraph, const std::string_view name, spa::Vec2I position): uniqueId(pGraph->uniqueId++), name(name), pGraph(pGraph), position(position)
		{}
	}

	namespace ser
	{
		void SerializeWrite(const StreamWriter& sw, const scr::ScriptNode& data, const bool binary,
			const std::string& name)
		{
			sw.Serialize(data.uniqueId, binary);
			sw.Serialize(data.pinIds, binary);
			sw.Serialize(data.childrenIds, binary);
			sw.Serialize(data.name, binary);
			sw.Serialize(data.position, binary);
		}

		void SerializeRead(const StreamReader& sr, scr::ScriptNode& data, const bool binary,
			ExtraDataPack* pExtraData)
		{
			pExtraData->Get("pGraph").Get((void*&)data.pGraph);
			sr.ReadSerialized(data.uniqueId, binary, pExtraData);
			sr.ReadSerialized(data.pinIds, binary, pExtraData);
			sr.ReadSerialized(data.childrenIds, binary, pExtraData);
			sr.ReadSerialized(data.name, binary, pExtraData);
			sr.ReadSerialized(data.position, binary, pExtraData);
		}
	}
}

