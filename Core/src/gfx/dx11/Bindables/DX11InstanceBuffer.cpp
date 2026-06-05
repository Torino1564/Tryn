#include <Core/src/gfx/dx11/Bindables/DX11InstanceBuffer.h>

namespace tryn::gfx::dx11
{
	DX11InstanceBuffer::DX11InstanceBuffer(const Graphics& gfx, const ConstantBufferLayout::Node& node, int slot, std::size_t numInstances)
		: gfx(gfx)
	{
		this->type = GraphicAPI::DX11;
		ConstantBufferLayout layout;
		layout.Append(cbType::Array, "InstanceArray");
		this->slot = slot;
		layout["InstanceArray"].Set(arrayElement, numInstances);
		layout.Solidify();
		this->pCPUBuffer = std::make_shared<ConstantBuffer>(std::move(layout));
		this->gpuSize = IBufferBase<Type, Policy>::pCPUBuffer->Size();
		InitDynamicCBufferOnGPU();
	}

	void DX11InstanceBuffer::Resize(std::size_t newSize)
	{
		if (newSize == gpuSize) return;
		pCPUBuffer->Resize(newSize);
		GPUSizeChanges();
	}
	
	void DX11InstanceBuffer::GPUSizeChanges()
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;
		cbd.ByteWidth = (UINT)this->pCPUBuffer->ByteSize();
		gfx.GetDevice().CreateBuffer(&cbd, nullptr, &pBuffer) >> chk;
	}

	void DX11InstanceBuffer::Bind()
	{
		gfx.GetContext().Bind(*this);
	}

	void DX11InstanceBuffer::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		Bind_(*static_cast<const DX11Context*>(&context)->GetContext());
	}

	void DX11InstanceBuffer::Bind_(ID3D11DeviceContext& context)
	{
		if (this->pCPUBuffer->Dirty())
		{
			Update(context);
			this->pCPUBuffer->SetClean();
		}
		context.VSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
	}

	void DX11InstanceBuffer::Update(ID3D11DeviceContext& context)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		context.Map(pBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped);
		memcpy(mapped.pData, this->pCPUBuffer->Data(), this->pCPUBuffer->ByteSize());
		context.Unmap(pBuffer.Get(), 0u);
	}

	void DX11InstanceBuffer::Update()
	{
		Update(gfx.GetContext());
	}


}