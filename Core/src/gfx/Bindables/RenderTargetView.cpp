#include "RenderTargetView.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx 
{
    std::shared_ptr<IRenderTargetView> IRenderTargetView::Resolve(IGraphics& gfx, const spa::DimensionsI dimensions)
    {
        return BindablePool::Resolve<IRenderTargetView>(gfx, dimensions);
    }
}
