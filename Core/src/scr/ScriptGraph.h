#pragma once

namespace tryn::scr
{
	class ScriptNode;

	class ScriptGraph
	{
	public:
		friend class ScriptNode;

		void FillVariables(class VariableManager*) const;

	private:
		std::vector<std::unique_ptr<ScriptNode>> nodes;
		uint16_t currentNodeId;
	};
}