#include "DX11ConstantBuffer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11ConstantBuffer::DX11ConstantBuffer(Graphics& gfx, ConstantBufferLayout&& cbl)
		:
		gfx(gfx)
	{
		trynass_msg(cbl.IsSolid(), L"ConstantBuffer cannot be created with a non solidified layout!");

		layout = std::move(cbl);
		buffer.resize(layout.Size());

		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;
		cbd.ByteWidth = (UINT)layout.Size();
		D3D11_SUBRESOURCE_DATA csrd = {};
		csrd.pSysMem = Data();

		gfx.GetDevice()->CreateBuffer(&cbd, &csrd, &pCBuff) >> chk;
	}
	void DX11ConstantBuffer::Bind()
	{
		if (dirty)
		{
			Update();
			dirty = false;
		}
		gfx.GetContext()->VSSetConstantBuffers(0u, 1u, pCBuff.GetAddressOf());
	}
	char* DX11ConstantBuffer::Data()
	{
		return buffer.data();
	}
	void DX11ConstantBuffer::Update()
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		gfx.GetContext()->Map(
			pCBuff.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);
		memcpy(msr.pData, Data(), buffer.size());
		gfx.GetContext()->Unmap(pCBuff.Get(), 0u);
	}
}