#include "DX11VertexBuffer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <d3d11.h>

namespace tryn::gfx::dx11
{
	DX11VertexBuffer::DX11VertexBuffer(Graphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer)
		:
		gfx(gfx)
	{
		pCPUBuffer = cpuBuffer;
	}
	void DX11VertexBuffer::Bind()
	{
		BindSlotted(0,1);
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
	std::vector<char> DX11VertexBuffer::GetLayoutFromVB() const
	{
		return GetSlottedLayoutFromVB(0);
	}
	std::vector<char> DX11VertexBuffer::GetSlottedLayoutFromVB(int slot) const
	{
		const auto& vLayout = ConstGet().GetLayout();
		const auto descSize = vLayout.GetElementCount();
		const auto charVectorSize = descSize * sizeof(D3D11_INPUT_ELEMENT_DESC);

		std::vector<char> layout;
		layout.resize(charVectorSize);

		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutt;
		layout.resize(descSize);
		for (int i = 0; i < descSize; i++)
		{
			auto fakePtr = reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(layout.data() + sizeof(D3D11_INPUT_ELEMENT_DESC) * i);
			fakePtr->SemanticName = vLayout.Elements[i].first.GetName();
			fakePtr->SemanticIndex = vLayout.Elements[i].second;
			fakePtr->Format = MapDXGIFormat(vLayout.Elements[i].first.GetFormat());
			fakePtr->InputSlot = (UINT)slot;
			fakePtr->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			fakePtr->AlignedByteOffset = (UINT)vLayout.Elements[i].first.GetOffset();
			fakePtr->InstanceDataStepRate = 0u;
		}

		return layout;
	}
	void DX11VertexBuffer::BindSlotted(int slot, int buffCount)
	{
		if (Get().GetDirty())
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
		const UINT stride = (UINT)Get().Stride();
		const UINT offset = 0u;
		gfx.GetContext()->IASetVertexBuffers((UINT)slot, (UINT)buffCount, pBuffer.GetAddressOf(), &stride, &offset);
	}
}

