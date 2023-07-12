#include "DX11PolyVBuffer.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11PolyVBuffer::DX11PolyVBuffer(Graphics& gfx, std::vector<std::variant<std::pair<std::string, VertexBuffer>, std::shared_ptr<IVertexBuffer>>> CpuVBs, std::string tag)
		:
		gfx(gfx)
	{
		this->tag = tag;
		type = GraphicAPI::DX11;
		for (auto& buffer : CpuVBs)
		{
			if (std::holds_alternative<std::shared_ptr<IVertexBuffer>>(buffer))
			{
				decltype(auto) ptr = std::get<std::shared_ptr<IVertexBuffer>>(buffer);
				Append(ptr);
			}
			else
			{
				auto [tag, vb] = std::get<std::pair<std::string, VertexBuffer>>(buffer);
				Append(tag, std::forward<VertexBuffer>(vb));
			}
		}
	}

	void DX11PolyVBuffer::Append(std::string tag, VertexBuffer&& vb)
	{
		std::shared_ptr<IVertexBuffer> vertexBuffer = BindablePool::Resolve<IVertexBuffer>(gfx, std::forward<VertexBuffer>(vb), tag);
		slots.push_back(vertexBuffer);
	}

	void DX11PolyVBuffer::Append(std::shared_ptr<IVertexBuffer> pVB)
	{
		slots.push_back(pVB);
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