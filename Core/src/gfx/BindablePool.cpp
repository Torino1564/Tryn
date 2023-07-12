#include "BindablePool.h"
#include <Core/src/gfx/Bindables/BindableBase.h>

namespace tryn::gfx
{
    //std::shared_ptr<IVertexBuffer> BindablePool::ResolveVertexBuffer(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer)
    //{
    //    // Gets the key that would be generated from the queried resource
    //    const auto key = IVertexBuffer::GenerateID(gfx,cpuBuffer);
    //    const auto it = Get().pool.find(key);
    //    if (it == Get().pool.end())
    //    {
    //        // The key doesnt exist in the map, have to add a new element
    //        // Get ptr to newly created element
    //        std::shared_ptr<IVertexBuffer> bind = gfx.CreateVertexBuffer(cpuBuffer);
    //        // Assign a weak ptr to the map
    //        std::weak_ptr<IVertexBuffer> weakPtr = bind;
    //        Get().pool[key] = weakPtr;

    //        return bind;
    //    }
    //    else
    //    {
    //        // The key exists, returns a shared ptr to the queried resource
    //        std::shared_ptr<IBindable> bind = it->second.lock();
    //        return std::static_pointer_cast<IVertexBuffer>(bind);
    //    }
    //}
}

