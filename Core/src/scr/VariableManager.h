#pragma once

namespace tryn::scr
{
	class ScriptVariableBag;

	class VariableManager
	{
	public:
		VariableManager() = delete;
		VariableManager(ScriptVariableBag* instance, ScriptVariableBag* component);

		[[nodiscard]] ScriptVariableBag& GetComponentBag() const;
		[[nodiscard]] ScriptVariableBag& GetInstanceBag() const;

	private:
		ScriptVariableBag* pInstanceVariables = nullptr;
		ScriptVariableBag* pComponentVariables = nullptr;
	};
}
