#include <TrynEditor/src/App/TypeRegister.h>

using namespace tryn::ed;

__declspec(dllexport) bool GetRegister(TypeRegister* pReg)
{
	if (!pReg)
		return false;

	// Begin type registering
	// End type registering

	return true;
}