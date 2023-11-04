#include "DX11ConstantBuffer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11VtxConstantBuffer::DX11VtxConstantBuffer(Graphics& gfx, ConstantBufferLayout&& cbl, int slot, std::string tag)
		:
		gfx(gfx)
	{
		this->slot = slot;
		this->tag = tag;

		trynass_msg(cbl.IsSolid(), L"ConstantBuffer cannot be created with a non solidified layout!");
		type = GraphicAPI::DX11;
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

		gfx.GetDevice().CreateBuffer(&cbd, &csrd, &pCBuff) >> chk;
	}
	void DX11VtxConstantBuffer::Bind()
	{
		if (dirty)
		{
			Update();
			dirty = false;
		}
		gfx.GetContext().VSSetConstantBuffers(slot, 1u, pCBuff.GetAddressOf());
	}
	void DX11VtxConstantBuffer::Bind(IContext& context)
	{
		gfx.AssertContextCoherence(context);
		auto dx11context = static_cast<DX11Context*>(&context);
		if (dirty)
		{
			Update();
			dirty = false;
		}
		dx11context->GetContext().VSSetConstantBuffers(slot, 1u, pCBuff.GetAddressOf());
	}
	char* DX11VtxConstantBuffer::Data()
	{
		return buffer.data();
	}
	void DX11VtxConstantBuffer::Update()
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		gfx.GetContext().Map(
			pCBuff.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);
		memcpy(msr.pData, Data(), buffer.size());
		gfx.GetContext().Unmap(pCBuff.Get(), 0u);
	}

	DX11PxConstantBuffer::DX11PxConstantBuffer(Graphics& gfx, ConstantBufferLayout&& cbl, int slot, std::string tag)
		:
		gfx(gfx)
	{
		this->slot = slot;
		this->tag = tag;

		trynass_msg(cbl.IsSolid(), L"ConstantBuffer cannot be created with a non solidified layout!");
		type = GraphicAPI::DX11;
		layout = std::move(cbl);
		buffer.resize(layout.Size());

		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;
		cbd.ByteWidth = static_cast<UINT>(layout.Size());
		D3D11_SUBRESOURCE_DATA csrd = {};
		csrd.pSysMem = Data();

		gfx.GetDevice().CreateBuffer(&cbd, &csrd, &pCBuff) >> chk;
	}
	void DX11PxConstantBuffer::Bind()
	{
		if (dirty)
		{
			Update();
			dirty = false;
		}
		gfx.GetContext().PSSetConstantBuffers(slot, 1u, pCBuff.GetAddressOf());
	}
	void DX11PxConstantBuffer::Bind(IContext& context)
	{
		gfx.AssertContextCoherence(context);
		auto& dx11context = static_cast<DX11Context*>(&context)->GetContext();
		if (dirty)
		{
			Update();
			dirty = false;
		}
		dx11context.PSSetConstantBuffers(slot, 1u, pCBuff.GetAddressOf());
	}
	char* DX11PxConstantBuffer::Data()
	{
		return buffer.data();
	}
	void DX11PxConstantBuffer::Update()
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		gfx.GetContext().Map(
			pCBuff.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);
		memcpy(msr.pData, Data(), buffer.size());
		gfx.GetContext().Unmap(pCBuff.Get(), 0u);
	}
}