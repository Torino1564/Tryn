#include <iostream>
#include <Core/src/ioc/Container.h>
#include <Core/src/log/Log.h>
#include <Core/src/log/SeverityLevelPolicy.h>

using namespace tryn;
using namespace std::string_literals;

void Boot()
{
	log::Boot();
}

int main()
{
	Boot();



	trylog.fatal(L"Failure!");
	trylog.warn(L"warning!");
	trylog.error(L"Error!");

	return 0;
}