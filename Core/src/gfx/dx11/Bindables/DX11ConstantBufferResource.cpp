#include <Core/src/gfx/dx11/Bindables/DX11ConstantBufferResource.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/Dx11Context.h>

namespace tryn::gfx::dx11
{
	DX11ConstantBufferResource::DX11ConstantBufferResource(const Graphics& gfx, const ConstantBufferLayout& cbl, IConstantBufferResource::Type type, int slot, const std::string& tag)
		: gfx(gfx)
	{
		this->type = GraphicAPI::DX11;
		this->bindType = type;
		this->slot = slot;
		this->tag = tag;
		trynass_msg(cbl.IsSolid(), L"ConstantBuffer cannot be created with a non solidified layout!");
		this->pCPUBuffer = std::make_shared<ConstantBuffer>(cbl);
	}

	void DX11ConstantBufferResource::Bind()
	{
		Bind(gfx.GetContextInterface());
	}

	void DX11ConstantBufferResource::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		Bind_(static_cast<const DX11Context*>(&context)->GetContext());
	}

	void DX11ConstantBufferResource::Bind_(ID3D11DeviceContext& context)
	{
		if (this->pCPUBuffer->Dirty())
		{
			Update(context);
			this->pCPUBuffer->SetClean();
		}
		switch (bindType)
		{
		case Type::Pixel:
			context.PSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
			break;
		case Type::Vertex:
			context.VSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
			break;
		default:
			trynchk_fail.msg(L"Invalid constant buffer type");
		}
	}

	void DX11ConstantBufferResource::Update()
	{
		Update(gfx.GetContext());
	}

	void DX11ConstantBufferResource::Update(ID3D11DeviceContext& context)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		context.Map(pBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped);
		memcpy(mapped.pData, this->pCPUBuffer->Data(), this->pCPUBuffer->Size());
		context.Unmap(pBuffer.Get(), 0u);
	}

	void DX11ConstantBufferResource::InitDynamicCBufferOnGPU()
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;
		cbd.ByteWidth = (UINT)this->pCPUBuffer->Size();
		D3D11_SUBRESOURCE_DATA csrd = {};
		csrd.pSysMem = this->pCPUBuffer->Data();

		gfx.GetDevice().CreateBuffer(&cbd, &csrd, &pBuffer) >> chk;
	}

	void DX11ConstantBufferResource::Resize(std::size_t newSize)
	{
		this->pCPUBuffer->Resize(newSize);
		InitDynamicCBufferOnGPU();
	}
}