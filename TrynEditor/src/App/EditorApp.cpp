#include "EditorApp.h"
#include <TrynEditor/third/imgui-node-editor-0.9.3/imgui_node_editor.h>

namespace ned = ax::NodeEditor;

namespace tryn::ed
{
	struct PinInfo;

	struct PinInfo
    {
        unsigned long long parentId;
        std::string name;
        ned::PinKind kind;
        ned::PinId id;
    };

    struct Link
    {
        ned::LinkId Id;
        ned::PinId  InputId;
        ned::PinId  OutputId;

        Link(const ned::LinkId id, const PinInfo pin1, const PinInfo pin2)
	        :
        Id(id)
        {
	        if (pin1.kind == ned::PinKind::Input)
	        {
		        InputId = pin1.id;
                OutputId = pin2.id;
	        }
            else
            {
	            InputId = pin2.id;
                OutputId = pin1.id;
            }
        }
    };

    void ImGuiEx_BeginColumn()
    {
        ImGui::BeginGroup();
    }

    void ImGuiEx_NextColumn()
    {
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void ImGuiEx_EndColumn()
    {
        ImGui::EndGroup();
    }

    struct Node
    {
        unsigned long long uniqueId;
        std::vector<PinInfo> pins;
        std::vector<uint16_t> childrenIds;
        std::string_view name;
        Node() = delete;
        void Submit()
        {
            if (!placed)
            {
                ned::SetNodePosition(uniqueId, ImVec2(10 + 50 * uniqueId, 10));
                placed = true;
            }
            ned::BeginNode(uniqueId);
            ImGui::Text(std::format("Node {}", name.data()).c_str());

            for (auto& pin : pins)
            {
                ned::BeginPin(pin.id, pin.kind);
                if (pin.kind == ned::PinKind::Input)
                    ImGui::Text(std::format("Input Pin {}", pin.name).c_str());  
                else
                    ImGui::Text(std::format("Output Pin {}", pin.name).c_str());

                ned::EndPin();
            }

            ned::EndNode();
        }
        bool placed = false;

        friend class TrynEditorApp;
    private:
        Node(const unsigned long long id, const std::string_view name) : uniqueId(id), name(name) {}
    };

    Node& TrynEditorApp::CreateNewNode(const std::string_view name, const int numberInputs, const int numberOutputs)
    {
        Node newVal(uniqueId++, name);

        for (int i = 0 ; i < numberInputs; i++)
        {
            PinInfo info{ .parentId = newVal.uniqueId, .name = "", .kind = ned::PinKind::Input, .id = uniqueId };
            pinIdToNodeId.insert({ uniqueId, info });
            newVal.pins.push_back(info);
            uniqueId++;
        }

        for (int i = 0; i < numberOutputs; i++)
        {
            PinInfo info{ .parentId = newVal.uniqueId, .name = "", .kind = ned::PinKind::Output, .id = uniqueId };
            pinIdToNodeId.insert({ uniqueId, info });
            newVal.pins.push_back(info);
            uniqueId++;
        }

        nodes.push_back(newVal);
        nodeIdToNodeIndex.insert({newVal.uniqueId, static_cast<uint16_t>(nodes.size() - 1)});

        return nodes.back();
    }

    void TrynEditorApp::CreateNewLink(ax::NodeEditor::LinkId id, const PinInfo pin1, const PinInfo pin2)
    {
	    m_Links.emplace_back(id, pin1, pin2);

        if (pin1.kind == ned::PinKind::Input)
        {
	        auto& outputNode = nodes[nodeIdToNodeIndex[pin2.parentId]];
    		auto& inputNode = nodes[nodeIdToNodeIndex[pin1.parentId]];

    		outputNode.childrenIds.push_back(nodeIdToNodeIndex[pin1.parentId]);
        }
        else
        {
	        auto& outputNode = nodes[nodeIdToNodeIndex[pin2.parentId]];
    		auto& inputNode = nodes[nodeIdToNodeIndex[pin1.parentId]];

    		outputNode.childrenIds.push_back(nodeIdToNodeIndex[pin1.parentId]);
        }
    	

    }

    TrynEditorApp::TrynEditorApp(const std::shared_ptr<win::IWindow>& pWnd, const std::shared_ptr<gfx::IGraphics>& pGfx)
        : App(pWnd, pGfx), pContext(ned::CreateEditor())
    {
        CreateNewNode("A", 3, 3);
        CreateNewNode("B", 1, 2);

        ECS().GetSystemManager().Finalize();
    }

    void TrynEditorApp::DoFrame()
	{
        auto& io = ImGui::GetIO();

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ImGui::Separator();

        ned::SetCurrentEditor(pContext);

        // Start interaction with editor.
        ned::Begin("Node Editor", ImVec2(0.0, 0.0f));

        //
        // 1) Commit known data to editor
        //

        for (auto& node : nodes)
        {
            node.Submit();
        }

        // Submit Links
        for (auto& linkInfo : m_Links)
            ned::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

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
                        const auto& inputPinInfo = pinIdToNodeId[inputPinId.Get()];
                        const auto& outputPinInfo = pinIdToNodeId[outputPinId.Get()];

                        bool accepted = true;

                        if (inputPinInfo.kind == outputPinInfo.kind)
                            accepted = false;

                        if (accepted)
                        {
	                        // Since we accepted new link, lets add one to our list of links.
                        	m_Links.push_back({ ned::LinkId(m_NextLinkId++), inputPinInfo, outputPinInfo });
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
                    for (auto& link : m_Links)
                    {
                        if (link.Id == deletnedLinkId)
                        { 
                            m_Links.erase(&link);
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

        if (m_FirstFrame)
            ned::NavigateToContent(0.0f);

        ned::SetCurrentEditor(nullptr);

        m_FirstFrame = false;

    	//ImGui::ShowMetricsWindow();
    }
}
