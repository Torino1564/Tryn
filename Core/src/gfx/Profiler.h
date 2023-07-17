#pragma once
#include <Core/src/utl/Timer.h>

#define PROFILE_SCOPE(name) tryn::utl::Timer ScopedTimer##__LINE__(name)