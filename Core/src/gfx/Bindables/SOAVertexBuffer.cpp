#include "TrynPCH.h"
#include "SOAVertexBuffer.h"

#include "IBufferBase.h"
#include "Core/src/gfx/BindablePool.h"

namespace tryn::gfx
{

	std::shared_ptr<ISOAVertexBuffer> ISOAVertexBuffer::Resolve(const IGraphics& gfx,
	                                                            const std::shared_ptr<IVertexShader>& pVS)
	{
		return BindablePool::Resolve<ISOAVertexBuffer>(gfx, pVS);
	}

	void ISOAVertexBuffer::Append(const std::shared_ptr<IVertexBuffer>& vertexBuffer, const std::string& name, uint16_t slot = 0)
	{
		if (pBuffers.contains(name))
		{
			trylog.warn(L"Cannot append the vertex buffer [" + utl::ToWide(name) + L"]");
		}
		else
		{
			dirty = true;
			AssertApiMatch(vertexBuffer);
			pBuffers.emplace(name, std::pair{vertexBuffer, slot});
		}
	}

	void ISOAVertexBuffer::AppendFrom(const ISOAVertexBuffer& soaVertexBuffer, const std::string& name, uint16_t slot)
	{
		if (const auto it = soaVertexBuffer.pBuffers.find(name); it != soaVertexBuffer.pBuffers.end())
		{
			dirty = true;
			auto& [buffer, _] = it->second;
			AssertApiMatch(buffer);
			pBuffers.emplace(name, std::pair{ buffer, slot });
		}
		else
		{
			trylog.warn(L"Unable to find the vertex buffer [" + utl::ToWide(name) + L"]");
		}
	}
}
