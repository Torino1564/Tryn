#include "TrynPCH.h"
#include "VariableManager.h"
#include "ScriptVariable.h"

namespace tryn::scr
{
	VariableManager::VariableManager(ScriptVariableBag* instance, ScriptVariableBag* component)
		:
	pInstanceVariables(instance), pComponentVariables(component)
	{
	}

	ScriptVariableBag& VariableManager::GetComponentBag() const
	{
		return *pComponentVariables;
	}

	ScriptVariableBag& VariableManager::GetInstanceBag() const
	{
		return *pInstanceVariables;
	}
}
