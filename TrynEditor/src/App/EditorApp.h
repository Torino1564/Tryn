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

		void LoadConfigs();

		ax::NodeEditor::EditorContext* pContext;
		bool m_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		std::unique_ptr<ScriptGraph> pGraph;

		bool debugging = false;
		uint16_t currentNodeId = 0;
		std::ostringstream oss;
		std::istringstream iss;
		ser::StreamWriter writer = ser::StreamWriter(oss);
		ser::StreamReader reader = ser::StreamReader(iss);

		std::unique_ptr<class Compiler> pCompiler;

		struct
		{
			std::string x64NativeToolsPath;
			std::string outputDirectory;
		} Configs;
	};
}