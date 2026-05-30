#pragma once
#include <any>
#include <Core/src/utl/ITypeRegister.h>

namespace tryn::gph
{
    using TTypeRegister = utl::ITypeRegister<std::any>;
}