#pragma once
#include <TrynEditor/src/Engine/Engine.h>
#include <vector>
#include <any>
#include <optional>

#include <Core/src/ser/StreamIO.h>

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
	struct ScriptGraph;

	static constexpr auto MAX_NAME_SIZE = 128;

	struct Variable
	{
		std::any var;
		utl::UUID_t uuid;
		std::string name;
		std::string typeName;
	};

	class TrynEditorApp final : public app::App
	{
	public:

		TrynEditorApp(const std::shared_ptr<win::IWindow>&, const std::shared_ptr<gfx::IGraphics>&);
		void DoFrame() override;
		void SerializeGraph();
		void LoadGraph();

		void LoadConfigs();
		void RMBMenu(ImVec2 pos);
		void NodeCreateMenu();
		ax::NodeEditor::EditorContext* pContext;
		bool m_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		std::unique_ptr<ScriptGraph> pGraph;

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
		void(*createFunc) (ScriptGraph* graph, TrynEditorApp* pEditor) = nullptr;
	};
}