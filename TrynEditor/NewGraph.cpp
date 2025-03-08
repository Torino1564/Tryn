#include <TrynEditor/src/App/TypeRegister.h>

using namespace tryn::ed;

extern "C" __declspec(dllexport) bool GetRegister(TypeRegister* pReg)
{
	if (!pReg)
		return false;

	// Begin type registering
pReg->RegisterType<bool>();
pReg->RegisterType<bool>();
	// End type registering

	return true;
}
