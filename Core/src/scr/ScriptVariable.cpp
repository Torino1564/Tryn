#include "TrynPCH.h"
#include "ScriptVariable.h"

namespace tryn::scr
{
	ScriptVariable::~ScriptVariable()
	{
		pDeleter(GetData());
	}

	void* ScriptVariable::GetData()
	{
		return (void*)rawData.data();
	}
}
