#pragma once
#include <any>
#include <ranges>
#include <Core/src/scr/ScriptNode.h>
#include <Core/src/scr/ScriptGraph.h>

namespace tryn::scr
{
	struct ConditionalNode : public ScriptNode
    {
    public:
        ConditionalNode() = default;
        template <typename T>
        static constexpr ConditionalNode Make(ScriptGraph* pGraph, const std::string& name, const std::string& varName, T&& value_);

        unsigned long long Execute() override;

        static void ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);

    protected:
	    ConditionalNode(ScriptGraph* pGraph, const std::string& name, const std::string& varName);

        std::function<bool(const std::any&)> checkFunc;
        uint16_t varIndex = 0;
        std::string varName;

        // pins

        uint16_t truePin = 0;
        uint16_t falsePin = 0;
    };

	template <typename T>
	constexpr ConditionalNode ConditionalNode::Make(ScriptGraph* pGraph, const std::string& name,
		const std::string& varName, T&& value_)
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
}
