#include "EditorApp.h"
#include <TrynEditor/third/imgui-node-editor-0.9.3/imgui_node_editor.h>
#include <functional>

namespace ned = ax::NodeEditor;

namespace tryn::ed
{
	struct PinInfo;

	struct PinInfo
    {
        unsigned long long parentId;
        unsigned long long linkedId;
        std::string name;
        ned::PinKind kind;
        ned::PinId id;
        bool linked = false;
    };

    struct Link
    {
        ned::LinkId Id;
        ned::PinId  InputId;
        ned::PinId  OutputId;
        TrynEditorApp* pEditor = nullptr;

        Link(const ned::LinkId id, const PinInfo pin1, const PinInfo pin2, TrynEditorApp* pEditor)
	        :
        Id(id), pEditor(pEditor)
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
        ~Link();
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
        virtual ~Node() = default;
        unsigned long long uniqueId;
        std::vector<PinInfo> pins;
        std::vector<uint16_t> childrenIds;
        std::string name;
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
        TrynEditorApp* pEditorApp;

        virtual unsigned long long Execute() { return uniqueId; }


        friend class TrynEditorApp;
        Node(Node&&) = default;
    protected:
        Node(TrynEditorApp* pEditor, const std::string_view name) : uniqueId(pEditor->uniqueId++), name(name), pEditorApp(pEditor) {}
    };

    struct EntryNode : public Node
    {
	    EntryNode(TrynEditorApp* pEditor, const std::string& name)
		    : Node (pEditor, name)
	    {
            // Output Pins
            {
                PinInfo info{ .parentId = this->uniqueId, .name = "Out", .kind = ned::PinKind::Output, .id = pEditor->uniqueId++ };
                pEditor->pinIdToNodeId.insert({ info.id.Get(), info });
                pins.push_back(std::move(info));
            }
	    }

        unsigned long long Execute() override
	    {
            return pins[0].linked ? pins[0].linkedId : uniqueId;
	    }
    };

    template <typename T>
    struct SetVarNode : public Node
    {
        SetVarNode(TrynEditorApp* editorApp, const std::string& name, const std::string& varname, T&& value) : Node(editorApp, name), varName(varname), value(std::forward<T>(value))
        {
            bool foundVar = false;
            for (auto [index, variable] : std::ranges::views::enumerate(pEditorApp->variables))
            {
	            if (variable.name == varname)
	            {
		            varIndex = index;
                    foundVar = true;
                    break;
	            }
            }

            if (!foundVar)
            {
	            // Add new variable
                pEditorApp->variables.push_back(Variable{.var = std::make_any<T>(), .name = varname});
                varIndex = static_cast<uint16_t>(pEditorApp->variables.size() - 1);
            }

            // Input Pin

	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = ned::PinKind::Input, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
	        }

            // Output Pin
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "Out", .kind = ned::PinKind::Output, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
	        }

        }
        T& GetVar()
        {
            return std::any_cast<T&>(pEditorApp->variables[varIndex].var);
        }
        void SetVar()
        {
	        std::any_cast<T&>(pEditorApp->variables[varIndex].var) = value;
        }

        unsigned long long Execute() override
        {
			SetVar();
            return pins[1].linked ? pins[1].linkedId : uniqueId;
        }

    private:
        uint16_t varIndex;
        std::string varName;
        T value;
    };

    struct StateNode : public Node
    {
        StateNode(TrynEditorApp* editorApp, const std::string& name, std::vector<std::string> states_)
	        :
        Node(editorApp, name), numStates((uint16_t)states.size()), states(std::move(states_))
        {

            // Input Pin

            {
		        PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = ned::PinKind::Input, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
	        }

            // Output Pins

            for (const auto& state : states)
            {
	            PinInfo info{ .parentId = this->uniqueId, .name = "Out " + state, .kind = ned::PinKind::Output, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
            }
        }

        unsigned long long Execute() override
        {
	        return pins[currentState].linked ? pins[currentState].linkedId : uniqueId;
        }
    private:
        uint16_t numStates = 0;
        uint16_t currentState = 0;
        std::vector<std::string> states;
    };

    struct ConditionalNode : public Node
    {
    public:
        template <typename T>
        static constexpr ConditionalNode Make(TrynEditorApp* pEditor, const std::string& name, const std::string& varName, T&& value_)
        {
	        ConditionalNode retval(pEditor, name, varName);

            retval.checkFunc = [value_ = std::forward<T&&>(value_)](const std::any& value) -> bool
            {
	            static const auto ref = value_;
                const auto& val = std::any_cast<const T>(value);

                return val == ref;
            };

        	bool foundVar = false;
            for (auto [index, variable] : std::ranges::views::enumerate(pEditor->variables))
            {
	            if (variable.name == varName)
	            {
		            retval.varIndex = index;
                    foundVar = true;
                    break;
	            }
            }

            if (!foundVar)
            {
	            // Add new variable
                pEditor->variables.push_back(Variable{.var = std::make_any<T>(), .name = varName});
                retval.varIndex = static_cast<uint16_t>(pEditor->variables.size() - 1);
            }

            return retval;
        }

        unsigned long long Execute() override
        {
	        if (const auto& value = pEditorApp->variables[varIndex].var; checkFunc(value))
            {
	            return pins[truePin].linked ? pins[truePin].linkedId : uniqueId;
            }
            else
            {
	            return pins[falsePin].linked ? pins[falsePin].linkedId : uniqueId;
            }
        }

    protected:
	    ConditionalNode(TrynEditorApp* pEditor, const std::string& name, const std::string& varName) : Node(pEditor, name), varName(varName)
	    {
		    // Input Pin
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = ned::PinKind::Input, .id = pEditor->uniqueId++ };
            	pEditor->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
	        }

            // Output Pins
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "True", .kind = ned::PinKind::Output, .id = pEditor->uniqueId++ };
            	pEditor->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
                truePin = pins.size() - 1;
	        }
            {
		        PinInfo info{ .parentId = this->uniqueId, .name = "False", .kind = ned::PinKind::Output, .id = pEditor->uniqueId++ };
            	pEditor->pinIdToNodeId.insert({info.id.Get(), info});
            	pins.push_back(std::move(info));
                falsePin = pins.size() - 1;
	        }
	    }

        std::function<bool(const std::any&)> checkFunc;
        uint16_t varIndex = 0;
        std::string varName;

        // pins

        uint16_t truePin = 0;
        uint16_t falsePin = 0;
    };

    struct WaitNode : public Node
    {
	    WaitNode(TrynEditorApp* pEditor, const std::string& name, float timeInSeconds)
		    :
        Node(pEditor, name)
	    {
		    // Implement
	    }
    };


    Node& TrynEditorApp::CreateNewNode(const std::string_view name, const int numberInputs, const int numberOutputs)
    {
        Node newVal(this, name);

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

        nodes.push_back(std::move(std::make_unique<Node>(std::move(newVal))));
        nodeIdToNodeIndex.insert({newVal.uniqueId, static_cast<uint16_t>(nodes.size() - 1)});

        return *nodes.back();
    }

    Node& TrynEditorApp::CreateNewNode(std::unique_ptr<Node>&& newVal)
    {
    	nodes.push_back(std::forward<std::unique_ptr<Node>>(newVal));
        nodeIdToNodeIndex.insert({nodes.back()->uniqueId, static_cast<uint16_t>(nodes.size() - 1)});

        return *nodes.back();
    }

    void TrynEditorApp::CreateNewLink(ax::NodeEditor::LinkId id, const PinInfo pin1, const PinInfo pin2)
    {
	    m_Links.emplace_back(id, pin1, pin2, this);

        if (pin1.kind == ned::PinKind::Input)
        {
	        auto& outputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];
    		auto& inputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];

    		outputNode.childrenIds.push_back(nodeIdToNodeIndex[pin1.parentId]);
        }
        else
        {
	        auto& outputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];
    		auto& inputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];

    		outputNode.childrenIds.push_back(nodeIdToNodeIndex[pin1.parentId]);
        }
    }

    TrynEditorApp::TrynEditorApp(const std::shared_ptr<win::IWindow>& pWnd, const std::shared_ptr<gfx::IGraphics>& pGfx)
        : App(pWnd, pGfx), pContext(ned::CreateEditor())
    {
        CreateNewNode(std::make_unique<SetVarNode<int>>(this, "SetFunnyNumberTo69", "funnyNumber", 69));
        CreateNewNode(std::make_unique<StateNode>(this, "Logical Button - Secondary Fire", std::vector<std::string>{"RMBDown", "RMBUp"}));
        CreateNewNode(std::make_unique<ConditionalNode>(std::move(ConditionalNode::Make(this, "IsReadyToShootRocket", "RocketReady", true))));
        CreateNewNode(std::make_unique<EntryNode>(this, "Entry"));
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
            node->Submit();
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
                            CreateNewLink(m_NextLinkId++, inputPinInfo, outputPinInfo);
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
                    for (auto it = m_Links.begin(); it < m_Links.end(); it++)
                    {
                        if (it->Id == deletnedLinkId)
                        { 
                            m_Links.erase(it);
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

	Link::~Link()
    {
        trynass(pEditor);

        auto& children = pEditor->nodes[pEditor->nodeIdToNodeIndex[OutputId.Get()]]->childrenIds;

        for (auto it = children.begin(); it < children.end(); it++)
        {
	        if (*it == InputId.Get())
	        {
		        children.erase(it);
                break;
	        }
        }
    }

}
