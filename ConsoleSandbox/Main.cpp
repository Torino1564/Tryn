#include <iostream>
#include <Core/src/ioc/Container.h>
#include <Core/src/ioc/Singletons.h>
#include <Core/src/log/Log.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include <Core/src/utl/Assert.h>

using namespace tryn;
using namespace std::string_literals;

class Butts
{
public:
	Butts();
	int Test()
	{
		return 420;
	}
};

void Boot()
{
	log::Boot();
}

int main()
{
	Boot();

	auto test = ioc::Sing().Resolve<Butts>();
	std::cout << test->Test() << std::endl;

	trylog.fatal(L"Failure!");
	trylog.warn(L"warning!");
	trylog.error(L"Error!");

	return 0;
}