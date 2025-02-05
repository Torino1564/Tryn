#pragma once

namespace tryn::scr
{
	class ScriptNodeId;

	class ScriptNode
	{
	public:
		friend class ScriptGraph;
		virtual ~ScriptNode() = default;
		
	private:
		void Traverse(ScriptGraph&) const;
		virtual bool Condition() = 0;
		virtual void RegisterVariables(class VariableManager*) const = 0;
		virtual void Run(ScriptGraph&) const;
		std::vector<uint16_t> childrenIds;
	};
}