#include <TrynEditor/src/App/TypeRegister.h>

using namespace tryn::ed;

__declspec(dllexport) TypeRegister& GetRegister()
{
	thread_local TypeRegister reg;

	// Begin type registering
	// End type registering

	return reg;
}