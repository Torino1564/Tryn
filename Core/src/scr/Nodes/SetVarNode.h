#pragma once
#include <any>
#include <ranges>
#include "Core/src/scr/ScriptNode.h"
#include <Core/src/scr/ScriptGraph.h>

namespace tryn::scr
{
	struct SetVarNode : public ScriptNode
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

        unsigned long long Execute() override;

        static void ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);

    private:
        SetVarNode(ScriptGraph* editorApp, const std::string& name, const std::string& varname);

        uint16_t varIndex;
        std::string varName;
        void(*pSetVarFunc)(SetVarNode&, std::any&) = nullptr;
        std::any value;
    };
}
