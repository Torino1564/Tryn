#pragma once
#include <TrynEditor/src/Engine/Engine.h>
#include <vector>
#include <any>

namespace ax::NodeEditor
{
	struct EditorContext;
	struct LinkId;
}

namespace tryn::ed
{
	struct PinInfo;
	struct Link;
	struct Node;
	struct Variable
	{
		std::any var;
		std::string name;
	};

	class TrynEditorApp final : public app::App
	{
	public:

		TrynEditorApp(const std::shared_ptr<win::IWindow>&, const std::shared_ptr<gfx::IGraphics>&);
		void DoFrame() override;
		class Node& CreateNewNode(std::string_view name, int numberInputs, int numberOutputs);
		class Node& CreateNewNode(std::unique_ptr<Node>&& newVal);
		void CreateNewLink(ax::NodeEditor::LinkId, const PinInfo pin1, const PinInfo pin2);
		ax::NodeEditor::EditorContext* pContext;
		bool                 m_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		std::vector<Link>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
		int                  m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.

		std::unordered_map<unsigned long long, PinInfo> pinIdToNodeId;
		std::unordered_map<unsigned long long, std::uint16_t> nodeIdToNodeIndex;
		std::vector<Variable> variables;
		std::vector<std::unique_ptr<Node>> nodes;
		unsigned int uniqueId = 1;
	};
}