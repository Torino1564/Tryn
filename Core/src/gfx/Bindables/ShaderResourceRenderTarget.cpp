#include "ShaderResourceRenderTarget.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx 
{
    std::shared_ptr<IShaderResourceRenderTarget> IShaderResourceRenderTarget::Resolve(IGraphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
    {
        return BindablePool::Resolve<IShaderResourceRenderTarget>(gfx, dimensions, slot);
    }
}
