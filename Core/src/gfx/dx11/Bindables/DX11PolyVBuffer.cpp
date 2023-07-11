#include "DX11PolyVBuffer.h"
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11PolyVBuffer::DX11PolyVBuffer(Graphics& gfx)
		:
		gfx(gfx)
	{
		type = GraphicAPI::DX11;
	}

	void DX11PolyVBuffer::Append(std::shared_ptr<VertexBuffer> pVB)
	{
		std::shared_ptr<DX11VertexBuffer> vertexBuffer = std::make_shared<DX11VertexBuffer>(gfx, pVB);
		slots.push_back(vertexBuffer);
	}


	void DX11PolyVBuffer::Bind()
	{
		if (!initialized)
		{
			for (auto& vBuffer : slots)
			{
				if (vBuffer->Get().GetDirty())
				{
					vBuffer->Init();
				}
				strides.push_back((UINT)vBuffer->Get().Stride());
				offsets.push_back((UINT)0);
				auto dx11vb = std::dynamic_pointer_cast<DX11VertexBuffer>(vBuffer);
				buffArray.push_back(dx11vb->GetPtr());
			}
			initialized = true;
		}
		gfx.GetContext()->IASetVertexBuffers((UINT)0, (UINT)buffArray.size(), buffArray.data(), strides.data(), offsets.data());
	}

}