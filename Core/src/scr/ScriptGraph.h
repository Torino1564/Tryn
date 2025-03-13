#pragma once
#include "Core/src/spa/Vec2.h"
#include <Core/src/scr/Link.h>
#include <Core/src/scr/Variable.h>
#include <Core/src/scr/TypeRegister.h>

namespace tryn
{
    namespace scr
	{
        struct ScriptNode;
		struct PinInfo;
		struct ScriptGraph
		{
			ScriptGraph() = default;
			ScriptGraph(const std::string_view name) : name(name) {}
			ScriptNode& CreateNewNode(std::unique_ptr<ScriptNode>&& newVal, std::optional<spa::Vec2I> pos = std::nullopt);
			void CreateNewLink(unsigned long long linkId, PinInfo& pin1, PinInfo& pin2);
			~ScriptGraph()
			{
				m_Links.clear();
				nodes.clear();
				variables.clear();

				nodeIdToNodeIndex.clear();
				pinIdToInfo.clear();
			}
			std::vector<Link> m_Links;
			int m_NextLinkId = 100;
			std::unordered_map<unsigned long long, PinInfo> pinIdToInfo;
			std::unordered_map<unsigned long long, std::uint16_t> nodeIdToNodeIndex;
			std::vector<Variable> variables;
			std::vector<std::unique_ptr<ScriptNode>> nodes;
			unsigned int uniqueId = 1;
			std::optional<unsigned int> entryId;
			std::string name;
			std::unique_ptr<TypeRegister> pRegister;
			
			// execution stuff
			void ExecuteStep();
			unsigned long long currentNodeId = 0;
		};
	}
}
