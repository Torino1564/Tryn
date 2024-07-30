#include "TrynPCH.h"
#include "Singletons.h"

namespace tryn::ioc
{
	Singletons& Sing()
	{
		static Singletons sing;
		return sing;
	}
}