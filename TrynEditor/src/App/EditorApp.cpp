#include "EditorApp.h"
#include <TrynEditor/third/imgui-node-editor-0.9.3/imgui_node_editor.h>
#include <functional>
#include <filesystem>
#include <sstream>
#include <fstream>
#include "TypeRegister.h"
#include <TrynEditor/src/dll/Compiler.h>
#include <imgui_stdlib.h>

#include <dylib.hpp>

namespace ned = ax::NodeEditor;

namespace tryn::ed
{
    template <typename T>
    auto AdlIsMemberOfEd_impl_(T&&) -> void;

	struct PinInfo;
    struct ScriptGraph;

	struct ScriptGraph
    {
        ScriptGraph() = default;
        ScriptGraph(std::string_view name) : name(name) {}
    	Node& CreateNewNode(std::unique_ptr<Node>&& newVal, std::optional<spa::Vec2I> pos = std::nullopt);
		void CreateNewLink(ax::NodeEditor::LinkId id, PinInfo& pin1, PinInfo& pin2);
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
		std::vector<std::unique_ptr<Node>> nodes;
		unsigned int uniqueId = 1;
		std::optional<unsigned int> entryId;
        std::string name;
        std::unique_ptr<TypeRegister> pRegister;
        Compiler* pCompiler = nullptr;

        // execution stuff
        void ExecuteStep();
        unsigned long long currentNodeId = 0;
    };

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
        ScriptGraph* pGraph = nullptr;

        Link(ned::LinkId id, PinInfo& pin1, PinInfo& pin2, ScriptGraph* pGraph);
        Link& operator=(Link&& rhs) noexcept
        {
            Link temp(std::move(rhs));

            std::swap(Id, temp.Id);
            std::swap(InputId, temp.InputId);
            std::swap(OutputId, temp.OutputId);
            std::swap(pGraph, temp.pGraph);

            return *this;
        }
        Link(Link&& rhs) noexcept
        {
	        Id = rhs.Id;
            InputId = rhs.InputId;
            OutputId = rhs.OutputId;
        	pGraph = std::exchange(rhs.pGraph, nullptr);

        }
        ~Link();

        Link() = default;
    };

    struct Node
    {
        virtual ~Node() = default;
        unsigned long long uniqueId;
        std::vector<uint16_t> pinIds;
        std::vector<uint16_t> childrenIds;
        std::string name;
        void Submit()
        {
            if (!placed)
            {
                ned::SetNodePosition(uniqueId, ned::ScreenToCanvas(ImVec2(position.x, position.y)));
                
                placed = true;
            }
            ned::BeginNode(uniqueId);
            ImGui::Text(std::format("Node {}", name.data()).c_str());

            for (auto& pinId : pinIds)
            {
                auto& pinInfo = pGraph->pinIdToInfo[pinId];
                ned::BeginPin(pinInfo.id, pinInfo.kind);
                if (pinInfo.kind == ned::PinKind::Input)
                    ImGui::Text(std::format("Input Pin {}", pinInfo.name).c_str());  
                else
                    ImGui::Text(std::format("Output Pin {}", pinInfo.name).c_str());

                ned::EndPin();
            }

            ned::EndNode();
        }
        bool placed = false;
        ScriptGraph* pGraph;
        spa::Vec2I position = {};
        virtual unsigned long long Execute() { return uniqueId; }

        static void ImGuiCreate(ScriptGraph* graph, TrynEditorApp* pEditor)
        {
        }

        friend class TrynEditorApp;
        Node() = default;
        Node(Node&&) = default;
    protected:
        Node(ScriptGraph* pGraph, const std::string_view name, spa::Vec2I position = {}) : uniqueId(pGraph->uniqueId++), name(name), pGraph(pGraph), position(position) {}
    };

    struct EntryNode : public Node
    {
        EntryNode() = default;
	    EntryNode(ScriptGraph* pGraph, const std::string& name)
		    : Node (pGraph, name)
	    {
            // Output Pins
            {
                PinInfo info{ .parentId = this->uniqueId, .name = "Out", .kind = ned::PinKind::Output, .id = pGraph->uniqueId++ };
                pGraph->pinIdToInfo.insert({ info.id.Get(), info });
                pinIds.push_back(info.id.Get());
            }

            trynass(!pGraph->entryId.has_value()).msg(L"The script already has an Entry Node!");

            pGraph->entryId.emplace(uniqueId);
	    }

        unsigned long long Execute() override
	    {
            const auto& info = pGraph->pinIdToInfo[pinIds[0]];
            return info.linked ? info.linkedId : uniqueId;
	    }

        static void ImGuiCreate(ScriptGraph* graph, TrynEditorApp* pEditor)
        {
            if(ImGui::Begin("EntryNodeCreate"))
            {
                static std::string name;
                if (ImGui::InputText("Node Name", &name)) {}

                if (ImGui::Button("Create"))
                {
	                graph->CreateNewNode(std::move(std::make_unique<EntryNode>(graph, std::string{name})), pEditor->lastRightClickPos);
                    pEditor->createFunc = nullptr;
                    name = {};
                }

	            ImGui::End();
            }

        }
    };

    struct SetVarNode : public Node
    {
        SetVarNode() = default;

        template <typename T>
        static SetVarNode Make(ScriptGraph* editorApp, const std::string& name, const std::string& varname, T&& value_)
        {
	        SetVarNode retval(editorApp, name, varname);


        	bool foundVar = false;
            for (auto [index, variable] : std::ranges::views::enumerate(retval.pGraph->variables))
            {
	            if (variable.name == varname)
	            {
		            retval.varIndex = index;
                    foundVar = true;
                    break;
	            }
            }

            if (!foundVar)
            {
	            // Add new variable
                retval.pGraph->variables.push_back(Variable{.var = std::make_any<T>(),.uuid = ZT_TYPE_UUID(T), .name = varname, .typeName = ZT_TYPE_OF(T).data()});
                retval.varIndex = static_cast<uint16_t>(retval.pGraph->variables.size() - 1);
            }

            retval.pSetVarFunc = [](SetVarNode& node, std::any& anyRef)
            {
	            node.GetVar<T>() = std::any_cast<T>(anyRef);
            };
            return retval;
        }

    	template <typename T>
        T& GetVar()
        {
            return std::any_cast<T&>(pGraph->variables[varIndex].var);
        }
        void SetVar()
        {
            pSetVarFunc(*this, value);
        }

        unsigned long long Execute() override
        {
			SetVar();
            const auto& info = pGraph->pinIdToInfo[pinIds[1]];
            return info.linked ? info.linkedId : uniqueId;
        }

        static void ImGuiCreate(ScriptGraph* graph, TrynEditorApp* pEditor)
        {
            if(ImGui::Begin("SetVarNodeCreate"))
            {
                static std::string name;
                if (ImGui::InputText("Node Name", &name)) {}

                static std::string varname;
                if (ImGui::InputText("Variable Name", &varname, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank)) {}
                static bool addNew = false;
                static bool checked = false;
                static auto s_it = graph->variables.end();
                if (ImGui::Button("Check"))
                {
                    checked = true;
	                const auto it = std::ranges::find_if(graph->variables, [](const auto& var)
	                {
		                return var.name == varname;
	                } );

                    if (it == graph->variables.end())
                    {
	                    // Add new variable
                        addNew = true;
                        s_it = graph->variables.end();
                    }
                    else
                    {
	                    addNew = false;
						s_it = it;
                    }
                }
                if (checked)
	            {
		            if(addNew)
		            {
		            	ImGui::Text("Variable not found in the script. Adding new variable.");
		            	static int currentElement = 0;
		            }
		            else
		            {
		            	ImGui::Text(std::format("Found existing variable: {} - Type: {}", s_it->name, s_it->typeName).c_str());
		            }
	            }

                ImGui::Text("Value to be set:");
                
                static std::variant<int, float, double, bool> inputBuffer = inputBuffer.emplace<int>();
                static const std::array items = {"int", "float", "double", "bool"};
                static int currentElement = 0;
                if (ImGui::Combo("Input Type", &currentElement, items.data(), items.size()))
                {
                	switch(currentElement)
	                {
	                case 0:
		                {
	                        inputBuffer = {};
	                        inputBuffer.emplace<int>();
			                break;
		                }
					case 1:
		                {
	                        inputBuffer = {};
                			inputBuffer.emplace<float>();
			                break;
		                }
					case 2:
		                {
	                        inputBuffer = {};
	                        inputBuffer.emplace<double>();
			                break;
		                }
					case 3:
		                {
	                        inputBuffer = {};
	                        inputBuffer.emplace<bool>();
			                break;
		                }
	                }
                }

                switch(currentElement)
                {
                case 0:
	                {
                        ImGui::InputInt("Int", &std::get<int>(inputBuffer));
		                break;
	                }
				case 1:
	                {
                        ImGui::InputFloat("Float", &std::get<float>(inputBuffer));
		                break;
	                }
				case 2:
	                {
                        ImGui::InputDouble("Double", &std::get<double>(inputBuffer));
		                break;
	                }
				case 3:
	                {
                        ImGui::Checkbox("True", &std::get<bool>(inputBuffer));
		                break;
	                }
                }

                if (ImGui::Button("Create"))
                {
	                switch(currentElement)
	                {
	                case 0:
		                {
	                        graph->CreateNewNode(std::make_unique<SetVarNode>(std::move(Make<int>(graph, {name}, {varname}, std::move(std::get<int>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
		                }
					case 1:
		                {
                			graph->CreateNewNode(std::make_unique<SetVarNode>(std::move(Make<float>(graph, {name}, {varname}, std::move(std::get<float>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
		                }
					case 2:
		                {
                			graph->CreateNewNode(std::make_unique<SetVarNode>(std::move(Make<double>(graph, {name}, {varname}, std::move(std::get<double>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
		                }
					case 3:
		                {
                			graph->CreateNewNode(std::make_unique<SetVarNode>(std::move(Make<bool>(graph, {name}, {varname}, std::move(std::get<bool>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
		                }
	                }

                	name = {};
                    varname = {};
                    pEditor->createFunc = nullptr;
                    checked = false;
                    addNew = false;
                }
	            ImGui::End();
            }
        }

    private:
        SetVarNode(ScriptGraph* editorApp, const std::string& name, const std::string& varname) : Node(editorApp, name), varName(varname)
        {
            // Input Pin
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = ned::PinKind::Input, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
	        }

            // Output Pin
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "Out", .kind = ned::PinKind::Output, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
	        }

        }

        uint16_t varIndex;
        std::string varName;
        void(*pSetVarFunc)(SetVarNode&, std::any&) = nullptr;
        std::any value;
    };

    struct StateNode : public Node
    {
        StateNode() = default;
        StateNode(ScriptGraph* editorApp, const std::string& name, std::vector<std::string> states_)
	        :
        Node(editorApp, name), numStates((uint16_t)states.size()), states(std::move(states_))
        {

            // Input Pin

            {
		        PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = ned::PinKind::Input, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
	        }

            // Output Pins

            for (const auto& state : states)
            {
	            PinInfo info{ .parentId = this->uniqueId, .name = "Out " + state, .kind = ned::PinKind::Output, .id = editorApp->uniqueId++ };
            	editorApp->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
            }
        }

        static void ImGuiCreate(ScriptGraph* graph, TrynEditorApp* pEditor)
        {
            if(ImGui::Begin("StateNodeCreate"))
            {
                static std::string name;
                if (ImGui::InputText("Node Name", &name)) {}

                static auto stateCount = 1;
                if (ImGui::InputInt("Number of States", &stateCount, 1, 3))
                {
                    if (stateCount <= 0)
                    {
                    	trylog.warn(L"The state count cannot be 0 or negative");
                        stateCount = 1;
                    }
                    if (stateCount >= 50)
                    {
	                    trylog.warn(L"The state count cannot exceed 50");
                        stateCount = 49;
                    }
                }
                static std::vector<std::string> states;
                states.resize(stateCount);
                ImGui::Text("States:");
                for (int i = 0; i < stateCount; i++)
                {
	                ImGui::InputText(std::to_string(i).c_str(), &states[i]);
                }

                if (ImGui::Button("Create"))
                {
	                graph->CreateNewNode(std::make_unique<StateNode>(graph, std::string{name}, std::vector<std::string>{states}), pEditor->lastRightClickPos);
                    name = {};
                    states = {};
                    pEditor->createFunc = nullptr;
                }

	            ImGui::End();
            }

        }

        unsigned long long Execute() override
        {
            const auto& info = pGraph->pinIdToInfo[pinIds[currentState]];
	        return info.linked ? info.linkedId : uniqueId;
        }
    private:
        uint16_t numStates = 0;
        uint16_t currentState = 1;
        std::vector<std::string> states;
    };

    struct ConditionalNode : public Node
    {
    public:
        ConditionalNode() = default;
        template <typename T>
        static constexpr ConditionalNode Make(ScriptGraph* pGraph, const std::string& name, const std::string& varName, T&& value_)
        {
	        ConditionalNode retval(pGraph, name, varName);

            retval.checkFunc = [value_ = std::forward<T&&>(value_)](const std::any& value) -> bool
            {
	            static const auto ref = value_;
                const auto& val = std::any_cast<const T>(value);

                return val == ref;
            };

        	bool foundVar = false;
            for (auto [index, variable] : std::ranges::views::enumerate(pGraph->variables))
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
                pGraph->variables.push_back(Variable{.var = std::make_any<T>(), .uuid = ZT_TYPE_UUID(T), .name = varName, .typeName = ZT_TYPE_OF(T).data()});
                retval.varIndex = static_cast<uint16_t>(pGraph->variables.size() - 1);
            }

            return retval;
        }

        unsigned long long Execute() override
        {
	        if (const auto& value = pGraph->variables[varIndex].var; checkFunc(value))
            {
				const auto& info = pGraph->pinIdToInfo[pinIds[truePin]];
	            return info.linked ? info.linkedId : uniqueId;
            }
            else
            {
				const auto& info = pGraph->pinIdToInfo[pinIds[falsePin]];
	            return info.linked ? info.linkedId : uniqueId;
            }
        }

        static void ImGuiCreate(ScriptGraph* graph, TrynEditorApp* pEditor)
        {
	        if(ImGui::Begin("ConditionalNodeCreate"))
			{
			    static std::string name;
			    if (ImGui::InputText("Node Name", &name)) {}

			    static std::string varname;
			    if (ImGui::InputText("Variable Name", &varname, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank)) {}
			    static bool addNew = false;
			    static bool checked = false;
			    static auto s_it = graph->variables.end();
			    if (ImGui::Button("Check"))
			    {
			        checked = true;
			        const auto it = std::ranges::find_if(graph->variables, [](const auto& var)
			        {
			            return var.name == varname;
			        } );

			        if (it == graph->variables.end())
			        {
			            // Add new variable
			            addNew = true;
			            s_it = graph->variables.end();
			        }
			        else
			        {
			            addNew = false;
						s_it = it;
			        }
			    }
			    if (checked)
			    {
			        if(addNew)
			        {
        				ImGui::Text("Variable not found in the script. Adding new variable.");
        				static int currentElement = 0;
			        }
			        else
			        {
        				ImGui::Text(std::format("Found existing variable: {} - Type: {}", s_it->name, s_it->typeName).c_str());
			        }
			    }

			    ImGui::Text("Value to compare:");
			    
			    static std::variant<int, float, double, bool, std::string> inputBuffer = inputBuffer.emplace<int>();
			    static const std::array items = {"int", "float", "double", "bool", "string"};
			    static int currentElement = 0;
			    if (ImGui::Combo("Input Type", &currentElement, items.data(), items.size()))
			    {
       				switch(currentElement)
			        {
			        case 0:
			            {
			                inputBuffer = {};
			                inputBuffer.emplace<int>();
			                break;
			            }
					case 1:
			            {
			                inputBuffer = {};
			        		inputBuffer.emplace<float>();
			                break;
			            }
					case 2:
			            {
			                inputBuffer = {};
			                inputBuffer.emplace<double>();
			                break;
			            }
					case 3:
			            {
			                inputBuffer = {};
			                inputBuffer.emplace<bool>();
			                break;
			            }
       				case 4:
			            {
			                inputBuffer = {};
			                inputBuffer.emplace<std::string>();
			                break;
			            }
			        }

			    }

			    switch(currentElement)
			    {
			    case 0:
			        {
			            ImGui::InputInt("Int", &std::get<int>(inputBuffer));
			            break;
			        }
				case 1:
			        {
			            ImGui::InputFloat("Float", &std::get<float>(inputBuffer));
			            break;
			        }
				case 2:
			        {
			            ImGui::InputDouble("Double", &std::get<double>(inputBuffer));
			            break;
			        }
				case 3:
			        {
			            ImGui::Checkbox("True", &std::get<bool>(inputBuffer));
			            break;
			        }
				case 4:
			        {
			            ImGui::InputText("True", &std::get<std::string>(inputBuffer));
			            break;
			        }
			    }

			    if (ImGui::Button("Create"))
			    {
			        switch(currentElement)
			        {
			        case 0:
			            {
			                graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<int>(graph, {name}, {varname}, std::move(std::get<int>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
			            }
					case 1:
			            {
			        		graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<float>(graph, {name}, {varname}, std::move(std::get<float>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
			            }
					case 2:
			            {
			        		graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<double>(graph, {name}, {varname}, std::move(std::get<double>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
			            }
					case 3:
			            {
			        		graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<bool>(graph, {name}, {varname}, std::move(std::get<bool>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
			            }
					case 4:
			            {
			        		graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<std::string>(graph, {name}, {varname}, std::move(std::get<std::string>(inputBuffer))))), pEditor->lastRightClickPos);
			                break;
			            }
			        }

       				name = {};
			        varname = {};
			        pEditor->createFunc = nullptr;
			        checked = false;
			        addNew = false;
			    }
			    ImGui::End();
			}
        }

    protected:
	    ConditionalNode(ScriptGraph* pGraph, const std::string& name, const std::string& varName) : Node(pGraph, name), varName(varName)
	    {
		    // Input Pin
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = ned::PinKind::Input, .id = pGraph->uniqueId++ };
            	pGraph->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
	        }

            // Output Pins
	        {
		        PinInfo info{ .parentId = this->uniqueId, .name = "True", .kind = ned::PinKind::Output, .id = pGraph->uniqueId++ };
            	pGraph->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
                truePin = pinIds.size() - 1;
	        }
            {
		        PinInfo info{ .parentId = this->uniqueId, .name = "False", .kind = ned::PinKind::Output, .id = pGraph->uniqueId++ };
            	pGraph->pinIdToInfo.insert({info.id.Get(), info});
            	pinIds.push_back(info.id.Get());
                falsePin = pinIds.size() - 1;
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
        WaitNode() = default;
	    WaitNode(ScriptGraph* pGraph, const std::string& name, float timeInSeconds)
		    :
        Node(pGraph, name)
	    {
		    // Implement
	    }
    };

    struct NodeRegister
    {
        struct NodeTypeInfo
        {
	        const char* name = nullptr;
            void (*pCreate)(ScriptGraph*, TrynEditorApp* pEditor) = nullptr;
        };

        // returns false if the type could not be added
        template <typename T>
        bool RegisterNodeType()
        {
        	static constexpr auto uuid = ZT_TYPE_UUID(T);

            if (const auto it = map.find(uuid); it != map.end())
                return false;

            map.insert({uuid, NodeTypeInfo{.name = ZT_TYPE_OF(T).data(), .pCreate = &T::ImGuiCreate}});
            return true;
        }
	    static NodeRegister& Get()
	    {
		    static NodeRegister singleton;
            return singleton;
	    }
        auto& Map()
        {
	        return map;
        }
    private:

        NodeRegister() = default;
        std::unordered_map<utl::UUID_t, NodeTypeInfo> map;
    };

    TrynEditorApp::TrynEditorApp(const std::shared_ptr<win::IWindow>& pWnd, const std::shared_ptr<gfx::IGraphics>& pGfx)
        : App(pWnd, pGfx), pContext(ned::CreateEditor())
    {
        LoadGraph();
        ECS().GetSystemManager().Finalize();
        pCompiler = std::make_unique<Compiler>();
        Resize({1920, 1080});
        wnd->SetResizableFlag(true);
        NodeRegister::Get().RegisterNodeType<EntryNode>();
        NodeRegister::Get().RegisterNodeType<ConditionalNode>();
        NodeRegister::Get().RegisterNodeType<WaitNode>();
        NodeRegister::Get().RegisterNodeType<StateNode>();

        NodeRegister::Get().RegisterNodeType<SetVarNode>();
        NodeRegister::Get().RegisterNodeType<SetVarNode>();
        NodeRegister::Get().RegisterNodeType<SetVarNode>();
    }

    void TrynEditorApp::DoFrame()
	{
        bool open = true;
        ImGui::ShowDemoWindow(&open);
        auto& io = ImGui::GetIO();

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

        ned::SetCurrentEditor(pContext);

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
        RMBMenu(rmbPos);
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

            node->Submit();

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
        for (auto& linkInfo : pGraph->m_Links)
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
                            pGraph->m_Links.emplace_back(pGraph->m_NextLinkId++, inputPinInfo, outputPinInfo, pGraph.get());
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
                    for (auto it = pGraph->m_Links.begin(); it < pGraph->m_Links.end(); it++)
                    {
                        if (it->Id == deletnedLinkId)
                        { 
                            pGraph->m_Links.erase(it);
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

    Link::Link(const ned::LinkId id, PinInfo& pin1, PinInfo& pin2, ScriptGraph* pGraph)
        :
        Id(id), pGraph(pGraph)
        {
            auto& nodes = pGraph->nodes;
            auto& nodeIdToNodeIndex = pGraph->nodeIdToNodeIndex;

		    if (pin1.kind == ned::PinKind::Input)
		    {
		        auto& outputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];
    			auto& inputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];

    			outputNode.childrenIds.push_back(inputNode.uniqueId);

                InputId = pin1.id;
                OutputId = pin2.id;
		    }
		    else
		    {
		        auto& outputNode = *nodes[nodeIdToNodeIndex[pin1.parentId]];
    			auto& inputNode = *nodes[nodeIdToNodeIndex[pin2.parentId]];

    			outputNode.childrenIds.push_back(inputNode.uniqueId);

                InputId = pin2.id;
                OutputId = pin1.id;
		    }
            pin1.linked = true;
			pin2.linked = true;
			pin1.linkedId = pin2.parentId;
			pin2.linkedId = pin1.parentId;
        }

	Link::~Link()
    {
        if (!pGraph)
            return;

    	auto& inputPinInfo = pGraph->pinIdToInfo[InputId.Get()];
        auto& outputPinInfo = pGraph->pinIdToInfo[OutputId.Get()];

        auto& children = pGraph->nodes[pGraph->nodeIdToNodeIndex[outputPinInfo.parentId]]->childrenIds;

        for (auto it = children.begin(); it < children.end(); it++)
        {
	        if (*it == inputPinInfo.parentId)
	        {
		        children.erase(it);
                break;
	        }
        }

        inputPinInfo.linked = false;
        outputPinInfo.linked = false;
    }

	Node& ScriptGraph::CreateNewNode(std::unique_ptr<Node>&& newVal, const std::optional<spa::Vec2I> pos)
	{
    	nodes.push_back(std::forward<std::unique_ptr<Node>>(newVal));
        nodeIdToNodeIndex.insert({nodes.back()->uniqueId, static_cast<uint16_t>(nodes.size() - 1)});

        if (pos.has_value())
        {
	        nodes.back()->position = pos.value();
        }

        return *nodes.back();
	}

    void ScriptGraph::CreateNewLink(ax::NodeEditor::LinkId id, PinInfo& pin1, PinInfo& pin2)
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

    void ScriptGraph::ExecuteStep()
    {
        const auto currentNodeIndex = this->nodeIdToNodeIndex[currentNodeId];

        currentNodeId = nodes[currentNodeIndex]->Execute();
    }
}

namespace tryn::ser
{
    void SerializeWrite(const StreamWriter& sw, const ed::Link& data, const bool binary, const std::string& name)
	{
        sw.Serialize(data.Id, binary);
        sw.Serialize(data.InputId, binary);
        sw.Serialize(data.OutputId, binary);
	}

    void SerializeRead(const StreamReader& sr, ed::Link& data_, const bool binary, ExtraDataPack* pExtraData = nullptr)
    {
        pExtraData->Get("pGraph").Get((void*&)data_.pGraph);
        sr.ReadSerialized(data_.Id, binary);
        sr.ReadSerialized(data_.InputId, binary);
        sr.ReadSerialized(data_.OutputId, binary);
    }

    void SerializeWrite(const StreamWriter& sw, const ed::Node& data, const bool binary, const std::string& name)
    {
        sw.Serialize(data.uniqueId, binary);
        sw.Serialize(data.pinIds, binary);
        sw.Serialize(data.childrenIds, binary);
        sw.Serialize(data.name, binary);
        sw.Serialize(data.position, binary);
    }

    void SerializeRead(const StreamReader& sr, ed::Node& data, const bool binary, ExtraDataPack* pExtraData = nullptr)
    {
        pExtraData->Get("pGraph").Get((void*&)data.pGraph);
        sr.ReadSerialized(data.uniqueId, binary, pExtraData);
        sr.ReadSerialized(data.pinIds, binary, pExtraData);
        sr.ReadSerialized(data.childrenIds, binary, pExtraData);
        sr.ReadSerialized(data.name, binary, pExtraData);
        sr.ReadSerialized(data.position, binary, pExtraData);
    }

    void SerializeWrite(const StreamWriter& sw, const ed::PinInfo& data, const bool binary, const std::string& name)
    {
        sw.Serialize(data.parentId, binary);
        sw.Serialize(data.linkedId, binary);
        sw.Serialize(data.name, binary);
        sw.Serialize(data.kind, binary);
        sw.Serialize(data.id.Get(), binary);
        sw.Serialize(data.linked, binary);
    }

    void SerializeRead(const StreamReader& sr, ed::PinInfo& data, const bool binary, ExtraDataPack* pExtraData = nullptr)
    {
        sr.ReadSerialized(data.parentId, binary, pExtraData);
        sr.ReadSerialized(data.linkedId, binary, pExtraData);
        sr.ReadSerialized(data.name, binary, pExtraData);
        sr.ReadSerialized(data.kind, binary, pExtraData);
        data.id = ned::PinId(sr.ReadSerialized<unsigned long long>(binary, pExtraData));
        sr.ReadSerialized(data.linked, binary, pExtraData);
    }

    void SerializeWrite(const StreamWriter& sw, const ed::Variable& data, const bool binary, const std::string& name)
    {
        sw.Serialize(data.name, binary);
        sw.Serialize(data.typeName, binary);
        sw.Serialize(data.uuid, binary);
    }

    void SerializeRead(const StreamReader& sr, ed::Variable& data, const bool binary, ExtraDataPack* pExtraData = nullptr)
    {
        ed::TypeRegister* pTypeRegister = nullptr;
        pExtraData->Get("pTypeRegister").Get((void*&)pTypeRegister);
        sr.ReadSerialized(data.name, binary, pExtraData);
        sr.ReadSerialized(data.typeName, binary, pExtraData);
        sr.ReadSerialized(data.uuid, binary, pExtraData);

        pTypeRegister->ConstructAny(data.var, data.uuid);
    }

    void SerializeWrite(const StreamWriter& sw, const ed::ScriptGraph& data, const bool binary, const std::string& name)
    {
        sw.Serialize(data.m_Links, binary);
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

        if (!std::filesystem::exists(templatePath))
        {
            trylog.fatal(L"File not found");
        }

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

        const auto dllPath = (workingDir / data.name).string();
        data.pCompiler->CompileToDLL((workingDir / (data.name + ".cpp")).string(), true);

        sw.Serialize(dllPath);
        sw.Serialize(data.variables, binary);
    }

    void SerializeRead(const StreamReader& sr, ed::ScriptGraph& data, const bool binary, ExtraDataPack* pExtraData)
    {
        sr.ReadSerialized(data.m_Links, binary, pExtraData);
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

        const auto pFunc = lib.get_function<bool(ed::TypeRegister*)>("GetRegister");
        data.pRegister = std::make_unique<ed::TypeRegister>();
        auto pRegister = data.pRegister.get();
        const auto er = pFunc(pRegister);

        pExtraData->AddElement(ElementDataView(*data.pRegister, "pTypeRegister"));
        sr.ReadSerialized(data.variables, binary, pExtraData);
    }
}

void ed::TrynEditorApp::SerializeGraph()
{
    std::ostringstream oss;
    pWriter = std::make_unique<ser::StreamWriter>(oss);
    pWriter->Serialize(*pGraph, true, "graphTest");

    std::ofstream file("serialize.txt");
    file << pWriter->GetStringStream().str();
    file.close();
}

void ed::TrynEditorApp::LoadGraph()
{
    std::ifstream file("serialize.txt");

    if (!file.is_open())
    {
	    pGraph.reset();
        pGraph = std::make_unique<ScriptGraph>("TestGraph");
        return;
    }

    const auto buf = file.rdbuf();
    std::stringstream ss; ss << buf;
    std::istringstream iss(ss.str());

    pReader = std::make_unique<ser::StreamReader>(iss);

    ScriptGraph* pNewGraph = new ScriptGraph;
    ser::ExtraDataPack extraData = {};
    extraData.AddElement(ser::ElementDataView(*pNewGraph, "pGraph"));
    pReader->ReadSerialized(*pNewGraph, true, &extraData);

    pGraph.reset(pNewGraph);
}

void ed::TrynEditorApp::LoadConfigs()
{
    
}

void ed::TrynEditorApp::RMBMenu(ImVec2 pos)
{
    ImGui::SetNextWindowPos(pos);
    ned::Suspend();
    if (ImGui::BeginPopup("Right Click Menu"))
    {
		ImGui::SeparatorText("Right click menu");
	    if (ImGui::BeginMenu("Create Node"))
	    {
	    	for (const auto& [uuid, nodeInfo] : NodeRegister::Get().Map())
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

void ed::TrynEditorApp::NodeCreateMenu()
{
    ned::Suspend();

    if (createFunc)
        createFunc(pGraph.get(), this);

    ned::Resume();
}

