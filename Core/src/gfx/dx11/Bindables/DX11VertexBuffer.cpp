#include "DX11VertexBuffer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <d3d11.h>

namespace tryn::gfx::dx11
{
	DX11VertexBuffer::DX11VertexBuffer(Graphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag)
		:
		gfx(gfx)
	{
		this->tag = tag;
		CPUBuffer = cpuBuffer;
		type = GraphicAPI::DX11;
	}
	void DX11VertexBuffer::Bind()
	{
		if (Get().GetDirty())
		{
			Init();
		}
		const UINT stride = (UINT)Get().Stride();
		const UINT offset = 0u;
		gfx.GetContext()->IASetVertexBuffers((UINT)0, (UINT)1, pBuffer.GetAddressOf(), &stride, &offset);
	}
	void DX11VertexBuffer::Init()
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = (UINT)Get().Stride();
		bd.ByteWidth = (UINT)Get().BufferSize();

		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = Get().Data();

		gfx.GetDevice()->CreateBuffer(&bd, &srd, &pBuffer) >> chk;

		Get().GetDirty() = false;
	}
	DXGI_FORMAT MapDXGIFormat(VertexLayout::Format format)
	{
		switch (format)
		{
		case VertexLayout::Format::Vec2F:
			return DXGI_FORMAT_R32G32_FLOAT;
			break;
		case VertexLayout::Format::Vec3F:
			return DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case VertexLayout::Format::Vec4F:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case VertexLayout::Format::Vec4C_UNorm:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
	std::vector<std::any> DX11VertexBuffer::GetLayoutFromVB() const
	{
		return GetSlottedLayoutFromVB(0);
	}
	std::vector<std::any> DX11VertexBuffer::GetSlottedLayoutFromVB(int slot) const
	{
		const auto& vLayout = ConstGet().GetLayout();
		const auto descSize = vLayout.GetElementCount();

		std::vector<std::any> layout;
		for (int i = 0; i < descSize; i++)
		{
			D3D11_INPUT_ELEMENT_DESC descriptor = {};
			descriptor.SemanticName = vLayout.Elements[i].first.GetName();
			descriptor.SemanticIndex = vLayout.Elements[i].second;
			descriptor.Format = MapDXGIFormat(vLayout.Elements[i].first.GetFormat());
			descriptor.InputSlot = (UINT)slot;
			descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descriptor.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			descriptor.InstanceDataStepRate = 0u;
			layout.push_back(descriptor);
		}

		return layout;
	}

	ID3D11Buffer* DX11VertexBuffer::GetPtr()
	{
		return *pBuffer.GetAddressOf();
	}

}