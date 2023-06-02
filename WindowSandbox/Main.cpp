#include <Core/src/log/Log.h>
#include <Core/src/ioc/Container.h>
#include <Core/src/win/WindowClass.h>
#include <Core/src/utl/Assert.h>
#include <memory>

using namespace tryn;

int main()
{
	trylog.warn(L"Test");

	auto pWinClass = std::make_unique<win::WindowClass>();
	trynchk(pWinClass);

	return 0;
}