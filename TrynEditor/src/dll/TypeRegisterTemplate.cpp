#include <Core/src/gph/TTypeRegister.h>

using namespace tryn::gph;

extern "C" __declspec(dllexport) bool GetRegister(TTypeRegister* pReg)
{
	if (!pReg)
		return false;

	// Begin type registering
	// End type registering

	return true;
}