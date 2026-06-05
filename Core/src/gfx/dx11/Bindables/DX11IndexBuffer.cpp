#include <Core/src/gfx/dx11/Bindables/DX11IndexBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/Dx11Context.h>

namespace tryn::gfx::dx11
{
	DX11IndexBuffer::DX11IndexBuffer(const Graphics& gfx, const std::shared_ptr<IndexBuffer>& cpuBuffer, const std::string& tag)
		: gfx(gfx)
	{
		this->type = GraphicAPI::DX11;
		this->tag = tag;
		trynass_msg(!cpuBuffer->Dirty(), L"Cant initialize a dirty Index Buffer!");
		this->pCPUBuffer = cpuBuffer;

		stride = this->pCPUBuffer->Stride();

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)this->pCPUBuffer->ByteSize();
		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = this->pCPUBuffer->Data();
		gfx.GetDevice().CreateBuffer(&bd, &srd, &pBuffer) >> chk;
	}

	void DX11IndexBuffer::Bind()
	{
		Bind(gfx.GetContextInterface());
	}

	void DX11IndexBuffer::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		Bind_(static_cast<const DX11Context*>(&context)->GetContext());
	}

	void DX11IndexBuffer::Bind_(ID3D11DeviceContext& context)
	{
		context.IASetIndexBuffer(pBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	void DX11IndexBuffer::Update()
	{
		Update(gfx.GetContext());
	}

	void DX11IndexBuffer::Update(ID3D11DeviceContext& context)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		context.Map(
			Data(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		) >> chk;
		memcpy(msr.pData, this->pCPUBuffer->Data(), this->pCPUBuffer->ByteSize());
		context.Unmap(Data(), 0u);
	}

	ID3D11Buffer* DX11IndexBuffer::Data() const
	{
		return pBuffer.Get();
	}

}


