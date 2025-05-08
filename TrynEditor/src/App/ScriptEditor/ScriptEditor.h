#pragma once
#include <functional>
#include <memory>
#include <Core/src/scr/ScriptGraph.h>
#include <Core/src/spa/Vec2.h>

#include "Core/src/ser/StreamIO.h"
#include "TrynEditor/src/App/Applet.h"

namespace ax::NodeEditor
{
	struct EditorContext;
	struct LinkId;
}

namespace tryn::ed
{
	class ScriptEditor : public Applet
	{
	public:
		ScriptEditor();
		~ScriptEditor();
		void Kernel() override;
		void SerializeGraph();
		void LoadGraph();
		void BackGroundImGuiWindow();
		void LoadConfigs();
		void RMBMenu(spa::Vec2F pos);
		void NodeCreateMenu();

		ax::NodeEditor::EditorContext* pContext;
		bool firstFrame = true;
		std::unique_ptr<scr::ScriptGraph> pGraph;
		bool debugging = false;
		unsigned long long currentNodeId = 0;
		std::unique_ptr<ser::StreamWriter> pWriter;
		std::unique_ptr<ser::StreamReader> pReader;
		std::unique_ptr<class Compiler> pCompiler;
		spa::Vec2I lastRightClickPos = {};
		void(*createFunc) (scr::ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>&) = nullptr;
		std::function<void(gph::TNode&)> submitBehaviour;
		gph::TNodeRegister<scr::ScriptGraph> nodeRegister;
	};
}