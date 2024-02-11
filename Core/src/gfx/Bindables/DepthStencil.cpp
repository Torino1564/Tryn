#include "DepthStencil.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
    std::shared_ptr<IDepthStencil> IDepthStencil::Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode)
    {
        return BindablePool::Resolve<IDepthStencil>(gfx, dimensions, mode);
    }
}
