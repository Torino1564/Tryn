#pragma once
#include <TrynEditor/src/Engine/Engine.h>
#include <vector>
#include <any>
#include <optional>

#include <Core/src/ser/StreamIO.h>

#include "Core/src/scr/ScriptNode.h"

namespace ax::NodeEditor
{
	struct EditorContext;
	struct LinkId;
}

namespace tryn::scr
{
	struct PinInfo;
	struct Link;
	struct ScriptNode;
	struct ScriptGraph;
}

namespace tryn::ed
{
	static constexpr auto MAX_NAME_SIZE = 128;

	class TrynEditorApp final : public app::App
	{
	public:

		TrynEditorApp(const std::shared_ptr<win::IWindow>&, const std::shared_ptr<gfx::IGraphics>&);
		void DoFrame() override;
		void SerializeGraph();
		void LoadGraph();
		void BackGroundImGuiWindow();
		void LoadConfigs();
		void RMBMenu(ImVec2 pos);
		void NodeCreateMenu();
		ax::NodeEditor::EditorContext* pContext;
		bool m_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		std::unique_ptr<scr::ScriptGraph> pGraph;

		bool debugging = false;
		uint16_t currentNodeId = 0;
		std::unique_ptr<ser::StreamWriter> pWriter;
		std::unique_ptr<ser::StreamReader> pReader;

		std::unique_ptr<class Compiler> pCompiler;

		struct
		{
			std::string x64NativeToolsPath;
			std::string outputDirectory;
		} Configs;

		spa::Vec2I lastRightClickPos = {};
		void(*createFunc) (scr::ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>&) = nullptr;
		std::function<void(scr::ScriptNode&)> submitBehaviour;
	};
}