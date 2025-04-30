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
		void SerializeWrite(const StreamWriter& sw, const gph::TNode& data, const bool binary,
			const std::string& name)
		{
			sw.Serialize(data.uniqueId, binary);
			sw.Serialize(data.pinIds, binary);
			sw.Serialize(data.childrenIds, binary);
			sw.Serialize(data.name, binary);
			sw.Serialize(data.position, binary);

			if (!binary)
			{
				auto& ss = sw.GetStringStream();
				nlohmann::json j;
				j["uniqueId"] = data.uniqueId;
				j["pinIds"] = data.pinIds;
				j["childrenIds"] = data.childrenIds;
				j["name"] = data.name;
				j["position"] = {data.position.x, data.position.y};

				ss << j.dump(2);
			}
		}

		void SerializeRead(const StreamReader& sr, gph::TNode& data, const bool binary,
			ExtraDataPack* pExtraData)
		{
			pExtraData->Get("pGraph").Get((void*&)data.pGraph);
			sr.ReadSerialized(data.uniqueId, binary, pExtraData);
			sr.ReadSerialized(data.pinIds, binary, pExtraData);
			sr.ReadSerialized(data.childrenIds, binary, pExtraData);
			sr.ReadSerialized(data.name, binary, pExtraData);
			sr.ReadSerialized(data.position, binary, pExtraData);

			if (!binary)
			{
				auto& iss = sr.GetStringStream();
				nlohmann::json j;
				iss >> j;

				data.uniqueId = j.at("uniqueId").get<unsigned long long>();
				data.pinIds = j.at("pinIds").get<std::vector<unsigned long long>>();
				data.childrenIds = j.at("childrenIds").get<std::vector<unsigned long long>>();
				data.name = j.at("name").get<std::string>();
				data.position.x = j.at("position")[0].get<int>();
				data.position.y = j.at("position")[1].get<int>();
			}
		}
	}
}

