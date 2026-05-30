#pragma once
#include <Core/src/app/App.h>

extern tryn::app::App* tryn::app::CreateApp(int argc, char** argv);

namespace tryn::app
{
	int Main(int argc, char** argv);
}