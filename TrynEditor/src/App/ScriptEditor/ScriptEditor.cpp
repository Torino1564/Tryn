#include "ScriptEditor.h"

#include <dylib.hpp>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <Core/third/imgui-node-editor-0.9.3/imgui_node_editor.h>
#include <Core/src/win/IWindow.h>
#include <Core/src/ser/StreamIO.h>
#include <TrynEditor/src/dll/Compiler.h>

#include "Core/src/scr/Nodes/ConditionalNode.h"
#include "Core/src/scr/Nodes/EntryNode.h"
#include "Core/src/scr/Nodes/SetVarNode.h"
#include "Core/src/scr/Nodes/StateNode.h"
#include "Core/src/scr/Nodes/WaitNode.h"

namespace ned = ax::NodeEditor;

namespace tryn::ed
{
	ScriptEditor::ScriptEditor()
		:
		pContext(ned::CreateEditor())
	{
        pCompiler = std::make_unique<Compiler>();
        nodeRegister.RegisterNodeType<scr::EntryNode>();
        nodeRegister.RegisterNodeType<scr::ConditionalNode>();
        nodeRegister.RegisterNodeType<scr::WaitNode>();
        nodeRegister.RegisterNodeType<scr::StateNode>();
        nodeRegister.RegisterNodeType<scr::SetVarNode>();

        submitBehaviour = [](gph::TNode& node_)
        {
            auto& node = static_cast<scr::ScriptNode&>(node_);
	        if (!node.placed)
			{
				ned::SetNodePosition(node.uniqueId, ned::ScreenToCanvas(ImVec2(node.position.x, node.position.y)));
	                
				node.placed = true;
			}
			ned::BeginNode(node.uniqueId);
			ImGui::Text(std::format("Node {}", node.name.data()).c_str());

			for (auto& pinId : node.pinIds)
			{
				auto& pinInfo = node.pGraph->pinIdToInfo[pinId];
				ned::BeginPin(pinInfo.id, (ned::PinKind)pinInfo.kind);
				if (pinInfo.kind == gph::PinKind::Input)
					ImGui::Text(std::format("Input Pin {}", pinInfo.name).c_str());  
				else
					ImGui::Text(std::format("Output Pin {}", pinInfo.name).c_str());

				ned::EndPin();
			}

			ned::EndNode();
        };
	}

	void ScriptEditor::DoFrame()
	{
		ImGui::Begin("ScriptEditor");

        const auto& io = ImGui::GetIO();

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ImGui::Separator();

        if (debugging || ImGui::ColorButton("Debug Script", {150, 150, 0, 1},0, {25, 25}))
        {
            if (!debugging)
            {
	            currentNodeId = pGraph->entryId.value();
                debugging = true;
            }

            currentNodeId = pGraph->nodes[pGraph->nodeIdToNodeIndex[currentNodeId]]->Execute();
        }

        if (ImGui::ColorButton("Stop Debugging", {0, 150, 150, 1},0, {25, 25}))
        {
            debugging = false;
            currentNodeId = pGraph->entryId.value();
        }

        if (ImGui::ColorButton("Save", { 150, 150, 0, 1 }, 0, { 25, 25 }))
        {
            SerializeGraph();
        }

        if (ImGui::ColorButton("Load", { 150, 50, 150, 1 }, 0, { 25, 25 }))
        {
            LoadGraph();
        }
        if (ImGui::MenuItem("New"))
        {
	        pGraph.release();
            pGraph = std::make_unique<scr::ScriptGraph>("NewGraph");
        }

        ned::SetCurrentEditor(pContext);

        if (pGraph)
	    {
		    // Start interaction with editor.
        	ned::Begin("Node Editor", ImVec2(0.0, 0.0f));

        	static ImVec2 rmbPos = {};

        	ned::Suspend();
        	if (ned::ShowBackgroundContextMenu())
        	{
        		ImGui::OpenPopup("Right Click Menu");
        		rmbPos = ImGui::GetMousePos();
        		lastRightClickPos = {(int)rmbPos.x, (int)rmbPos.y};
        	}
        	ned::Resume();
            RMBMenu({rmbPos.x, rmbPos.y});
        	NodeCreateMenu();
        	//
        	// 1) Commit known data to editor
        	//

        	static constexpr float rounding = 10.0f;
        	static constexpr float padding  = 12.0f;

        	for (auto& node : pGraph->nodes)
        	{
        		if (node->uniqueId == currentNodeId)
        			ned::PushStyleColor(ned::StyleColor_NodeBg,        ImColor(229, 129, 129, 200));
        		else
        			ned::PushStyleColor(ned::StyleColor_NodeBg,        ImColor(59, 59, 59, 200));

        		ned::PushStyleColor(ned::StyleColor_NodeBorder,    ImColor(125, 125, 125, 200));
        		ned::PushStyleColor(ned::StyleColor_PinRect,       ImColor(229, 229, 229, 60));
        		ned::PushStyleColor(ned::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

        		ned::PushStyleVar(ned::StyleVar_NodePadding,  ImVec4(0, 0, 0, 0));
        		ned::PushStyleVar(ned::StyleVar_NodeRounding, rounding);
        		ned::PushStyleVar(ned::StyleVar_SourceDirection, ImVec2(0.0f,  1.0f));
        		ned::PushStyleVar(ned::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        		ned::PushStyleVar(ned::StyleVar_LinkStrength, 0.0f);
        		ned::PushStyleVar(ned::StyleVar_PinBorderWidth, 1.0f);
        		ned::PushStyleVar(ned::StyleVar_PinRadius, 6.0f);

        		node->Submit(submitBehaviour);

        		ned::PopStyleVar(7);
        		ned::PopStyleColor(4);
        	}

        
        	const auto hoveredNodeId = ned::GetHoveredNode();
        	auto it = pGraph->nodeIdToNodeIndex.find(hoveredNodeId.Get());
        	if (it != pGraph->nodeIdToNodeIndex.end())
        	{
        		const auto pos = ned::GetNodePosition(hoveredNodeId);
        		pGraph->nodes[it->second]->position = {(int)pos.x, (int)pos.y}; 
        	}

        	// Submit Links
        	for (auto& pLink : pGraph->links)
        		ned::Link(pLink->Id, pLink->InputId, pLink->OutputId);

        	//
        	// 2) Handle interactions
        	//

        	// Handle creation action, returns true if editor want to create new object (node or link)
        	if (ned::BeginCreate())
        	{
        		ned::PinId inputPinId, outputPinId;
        		if (ned::QueryNewLink(&inputPinId, &outputPinId))
        		{
        			// QueryNewLink returns true if editor want to create new link between pins.
        			//
        			// Link can be created only for two valid pins, it is up to you to
        			// validate if connection make sense. Editor is happy to make any.
        			//
        			// Link always goes from input to output. User may choose to drag
        			// link from output pin or input pin. This determine which pin ids
        			// are valid and which are not:
        			//   * input valid, output invalid - user started to drag new ling from input pin
        			//   * input invalid, output valid - user started to drag new ling from output pin
        			//   * input valid, output valid   - user dragged link over other pin, can be validated

        			if (inputPinId && outputPinId) // both are valid, let's accept link
        			{
        				// ned::AcceptNewItem() return true when user release mouse button.
        				if (ned::AcceptNewItem())
        				{
        					auto& inputPinInfo = pGraph->pinIdToInfo[inputPinId.Get()];
        					auto& outputPinInfo = pGraph->pinIdToInfo[outputPinId.Get()];

        					bool accepted = true;

        					if (inputPinInfo.kind == outputPinInfo.kind)
        						accepted = false;

        					if (inputPinInfo.linked || outputPinInfo.linked)
        						accepted = false;

        					if (accepted)
        					{
        						// Since we accepted new link, lets add one to our list of links.
        						pGraph->links.push_back(std::move(std::make_unique<gph::TLink>(pGraph->m_NextLinkId++, inputPinInfo, outputPinInfo, pGraph.get())));
        					}
        				}

        				// You may choose to reject connection between these nodes
        				// by calling ned::RejectNewItem(). This will allow editor to give
        				// visual feedback by changing link thickness and color.
        			}
        		}
        	}
        	ned::EndCreate(); // Wraps up object creation action handling.


        	// Handle deletion action
        	if (ned::BeginDelete())
        	{
        		// There may be many links marked for deletion, let's loop over them.
        		ned::LinkId deletnedLinkId;
        		while (ned::QueryDeletedLink(&deletnedLinkId))
        		{
        			// If you agree that link can be deleted, accept deletion.
        			if (ned::AcceptDeletedItem())
        			{
        				// Then remove link from your data.
        				for (auto it = pGraph->links.begin(); it < pGraph->links.end(); it++)
        				{
                            auto& pLink = *it;
        					if (pLink->Id == deletnedLinkId.Get())
        					{ 
        						pGraph->links.erase(it);
        						break;
        					}
        				}
        			}

        			// You may reject link deletion by calling:
        			// ned::RejectDeletedItem();
        		}
        	}
        	ned::EndDelete(); // Wrap up deletion action

        	// End of interaction with editor.
        	ned::End();
	    }

        if (firstFrame)
            ned::NavigateToContent(0.0f);

        ned::SetCurrentEditor(nullptr);

        firstFrame = false;

        ImGui::End();
	}

	void ScriptEditor::SerializeGraph()
	{
	    auto [success, path] = win::SelectDirectory();
	    path = absolute(path);
	    std::ostringstream oss;
	    pWriter = std::make_unique<ser::StreamWriter>(oss);
	    auto& sw = *pWriter;
	    auto& data = *pGraph;

	    const bool binary = true;
	    sw.Serialize(data.links, binary);
		sw.Serialize(data.nodes, binary);
		sw.Serialize(data.entryId, binary);
		sw.Serialize(data.pinIdToInfo, binary);
		sw.Serialize(data.nodeIdToNodeIndex, binary);
		sw.Serialize(data.uniqueId, binary);
		sw.Serialize(data.m_NextLinkId, binary);
		sw.Serialize(data.name, binary);

		// Create dll with type register
		static auto workingDir = std::filesystem::current_path();
		std::filesystem::path templatePath = workingDir / "src" / "dll" / "TypeRegisterTemplate.cpp";

		templatePath = std::filesystem::absolute(templatePath);

		if (!exists(templatePath))
		{
			trylog.fatal(L"File not found");
		}

		{
			std::ifstream file(templatePath);
    		std::stringstream buffer;

    		std::string line;
    		while (std::getline(file, line)) {
    			buffer << line << '\n';
    			if (line.find("// Begin type registering") != std::string::npos) {
    				break; // Stop reading after this line
    			}
    		}

    		for (auto& variable : data.variables)
    		{
    			buffer << std::format("pReg->RegisterType<{}>();", variable.typeName) << "\n";
    		}

    		// Append the rest of the file
    		while (std::getline(file, line)) {
    			buffer << line << '\n';
    		}

    		std::ofstream outfile(data.name + ".cpp");

    		outfile << buffer.str();

    		outfile.close();
		}

		const auto dllPath = (workingDir / data.name).string();
		pCompiler->CompileToDLL((workingDir / (data.name + ".cpp")).string(), true);

		sw.Serialize(dllPath);
		sw.Serialize(data.variables, binary);

	    std::ofstream file(path / (pGraph->name + ".tscript"));
	    file << pWriter->GetStringStream().str();
	    file.close();
	}

	void ScriptEditor::LoadGraph()
	{
	    auto [success, path] = win::SelectFile(std::vector{ std::string{".tscript"}});
	    if (!success)
	    {
		    return;
	    }

	    std::ifstream file(path.c_str());

	    if (!file.is_open())
	    {
		    return;
	    }

	    const auto buf = file.rdbuf();
	    std::stringstream ss; ss << buf;
	    std::istringstream iss(ss.str());

	    pReader = std::make_unique<ser::StreamReader>(iss);

	    scr::ScriptGraph* pNewGraph = new scr::ScriptGraph("temp");
	    ser::ExtraDataPack extraData = {};
	    extraData.AddElement(ser::ElementDataView(*pNewGraph, "pGraph"));
	    
		{
	        auto& sr = *pReader;
	        auto& data = *pNewGraph;
	        auto pExtraData = &extraData;
	        const auto binary = true;

			sr.ReadSerialized(data.links, binary, pExtraData);
    		sr.ReadSerialized(data.nodes, binary, pExtraData);
    		sr.ReadSerialized(data.entryId, binary, pExtraData);
    		sr.ReadSerialized(data.pinIdToInfo, binary, pExtraData);
    		sr.ReadSerialized(data.nodeIdToNodeIndex, binary, pExtraData);
    		sr.ReadSerialized(data.uniqueId, binary, pExtraData);
    		sr.ReadSerialized(data.m_NextLinkId, binary, pExtraData);
    		sr.ReadSerialized(data.name, binary, pExtraData);

    		const auto dllPath = sr.ReadSerialized<std::string>(binary, pExtraData);

    		std::filesystem::path path(dllPath);

    		const auto lib = dylib(path.parent_path(), path.filename().string());

    		const auto pFunc = lib.get_function<bool(gph::TTypeRegister*)>("GetRegister");
    		data.pRegister = std::make_unique<gph::TTypeRegister>();
    		auto pRegister = data.pRegister.get();
    		const auto er = pFunc(pRegister);

    		pExtraData->AddElement(ser::ElementDataView(*data.pRegister, "pTypeRegister"));
    		sr.ReadSerialized(data.variables, binary, pExtraData);
		}

	    pGraph.reset(pNewGraph);
	}

	void ScriptEditor::BackGroundImGuiWindow()
	{

	}

	void ScriptEditor::LoadConfigs()
	{
	    
	}

	void ScriptEditor::RMBMenu(spa::Vec2F pos)
	{
	    ImGui::SetNextWindowPos({pos.x, pos.y});
	    ned::Suspend();
	    if (ImGui::BeginPopup("Right Click Menu"))
	    {
			ImGui::SeparatorText("Right click menu");
		    if (ImGui::BeginMenu("Create Node"))
		    {
	    		for (const auto& [uuid, nodeInfo] : nodeRegister.Map())
	    		{
	    			if (ImGui::Selectable(nodeInfo.name))
	    			{
	    				createFunc = nodeInfo.pCreate;
	    			}
	    		}
	    		ImGui::EndMenu();
		    }
	        ImGui::EndPopup();
	    }
	    ned::Resume();
	}

	void ScriptEditor::NodeCreateMenu()
	{
	    ned::Suspend();

	    static std::function<void()> finalBehaviour = [&]()
	        {
		        this->createFunc = nullptr;
	        };

	    if (createFunc)
	        createFunc(pGraph.get(), this->lastRightClickPos, finalBehaviour);

	    ned::Resume();
	}
}