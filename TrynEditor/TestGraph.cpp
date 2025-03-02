#include <TrynEditor/src/App/TypeRegister.h>

using namespace tryn::ed;

extern "C" __declspec(dllexport) bool GetRegister(TypeRegister* pReg)
{
	if (!pReg)
		return false;

	// Begin type registering
pReg->RegisterType<int>();
pReg->RegisterType<bool>();
pReg->RegisterType<double>();
	// End type registering

	return true;
}
