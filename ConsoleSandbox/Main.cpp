#include <iostream>
#include <Core/src/ioc/Container.h>
#include <Core/src/ioc/Singletons.h>
#include <Core/src/log/Log.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include <Core/src/utl/Assert.h>
#include <Core/src/win/WindowClass.h>

using namespace tryn;
using namespace std::string_literals;

class Butts
{
public:
	int Test()
	{
		return 420;
	}
};

void Boot()
{
	log::Boot();

	ioc::Get().Register<log::ISeverityLevelPolicy>([] {
		return std::make_shared<log::SeverityLevelPolicy>(log::Level::Info);
		});
}

int main()
{
	Boot();

	auto pWinClass = std::make_unique<win::WindowClass>();
	trynchk(pWinClass);

	trylog.error(L"No trace test").no_trace().no_line();
	trylog.warn(L"Traced warning").trace();

	return 0;
}