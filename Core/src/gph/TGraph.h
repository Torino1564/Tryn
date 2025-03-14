#pragma once
#include "PinInfo.h"
#include "Core/src/spa/Vec2.h"
#include "TTypeRegister.h"
#include "TLink.h"

namespace tryn::gph
{
	class TNode;

	class TGraph
	{
	public:
		TGraph() = default;
		TGraph(const std::string_view name) : name(name) {}
		TNode& CreateNewNode(std::unique_ptr<TNode>&& newVal, std::optional<spa::Vec2I> pos = std::nullopt);
		void CreateNewLink(unsigned long long linkId, PinInfo& pin1, PinInfo& pin2);
		void CreateNewLink(std::unique_ptr<TLink>&& newVal);
		virtual ~TGraph()
		{
			links.clear();
			nodes.clear();

			nodeIdToNodeIndex.clear();
			pinIdToInfo.clear();
		}
		std::vector<std::unique_ptr<TLink>> links;
		int m_NextLinkId = 100;
		std::unordered_map<unsigned long long, PinInfo> pinIdToInfo;
		std::unordered_map<unsigned long long, std::uint16_t> nodeIdToNodeIndex;
		std::vector<std::unique_ptr<TNode>> nodes;
		unsigned int uniqueId = 1;
		std::optional<unsigned int> entryId;
		std::string name;
		std::unique_ptr<TTypeRegister> pRegister;
		
		// execution stuff
		void ExecuteStep();
		unsigned long long currentNodeId = 0;
	};
}
