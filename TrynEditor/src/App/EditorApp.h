#pragma once
#include <TrynEditor/src/Engine/Engine.h>

namespace ax::NodeEditor
{
	struct EditorContext;
}

namespace tryn::ed
{
	struct LinkInfo;

	class TrynEditorApp final : public app::App
	{
	public:

		TrynEditorApp(const std::shared_ptr<win::IWindow>&, const std::shared_ptr<gfx::IGraphics>&);
		void DoFrame() override;
		class Node CreateNewNode(std::string_view name, int numberInputs, int numberOutputs);
		ax::NodeEditor::EditorContext* pContext;
		bool                 m_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
		int                  m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.

		std::unordered_map<unsigned long long, struct PinInfo> pinIdToNodeId;
		std::vector<struct Node> nodes;
		unsigned int uniqueId = 1;
	};
}