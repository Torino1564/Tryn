#include "Initialization.h"

void tryn::app::BootCore()
{
	ioc::Boot();
	log::Boot();
	win::Boot();
	gfx::Boot();
}
