
#include "DX11Buffer.h"

namespace tryn::gfx::dx11
{
	template <BufferType Type, CachingPolicy Policy>
	DX11Buffer<Type, Policy>::DX11Buffer(const Graphics& gfx, const std::shared_ptr<CPUBuffer>& pCpuBuffer, const std::string& tag)
		requires (Type == BufferType::Vertex || Type == BufferType::Index)
		:
		gfx(gfx)
	{
		IBufferBase<Type, Policy>::type = GraphicAPI::DX11;
		IBufferBase<Type, Policy>::tag = tag;

		trynass_msg(!pCpuBuffer->Dirty(), L"Cant initialize a dirty Vertex Buffer!");

		IBufferBase<Type, Policy>::pCPUBuffer = pCpuBuffer;
		stride = (UINT)IBufferBase<Type, Policy>::pCPUBuffer->Stride();

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = GetBindFlag<Type>();
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = stride;
		bd.ByteWidth = (UINT)IBufferBase<Type, Policy>::pCPUBuffer->ByteSize();

		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = IBufferBase<Type, Policy>::pCPUBuffer->Data();

		gfx.GetDevice().CreateBuffer(&bd, &srd, &pBuffer) >> chk;
	}

	template <BufferType Type, CachingPolicy Policy>
	DX11Buffer<Type, Policy>::DX11Buffer(const Graphics& gfx, const ConstantBufferLayout& cbl, int slot, const std::string& tag)
		requires (Type == BufferType::PxConstant || Type == BufferType::VtxConstant): gfx(gfx)
	{
		IBufferBase<Type, Policy>::type = GraphicAPI::DX11;
		IBufferBase<Type, Policy>::slot = slot;
		IBufferBase<Type, Policy>::tag = tag;

		trynass_msg(cbl.IsSolid(), L"ConstantBuffer cannot be created with a non solidified layout!");
		IBufferBase<Type, Policy>::pCPUBuffer = std::make_shared<ConstantBuffer>(cbl);

		InitDynamicCBufferOnGPU();
	}

	template <BufferType Type, CachingPolicy Policy>
	DX11Buffer<Type, Policy>::DX11Buffer(const Graphics& gfx, const ConstantBufferLayout::Node& arrayElement, int slot,
	                                     const std::size_t numInstances) requires (Type == BufferType::Instance): gfx(gfx)
	{
		IBufferBase<Type, Policy>::type = GraphicAPI::DX11;
		ConstantBufferLayout layout;
		layout.Append(cbType::Array, "InstanceArray");
		IBufferBase<Type, Policy>::slot = slot;
		layout["InstanceArray"].Set(arrayElement, numInstances);
		layout.Solidify();
		IBufferBase<Type, Policy>::pCPUBuffer = std::make_shared<ConstantBuffer>(std::move(layout));
		IBufferBase<Type, Policy>::gpuSize = IBufferBase<Type, Policy>::pCPUBuffer->Size();
		InitDynamicCBufferOnGPU();
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Resize(const std::size_t newSize)
	{
		Resize_(newSize);
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Resize_(const std::size_t newSize)
	{
		this->pCPUBuffer->Resize(newSize);
		InitDynamicCBufferOnGPU();
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::InitDynamicCBufferOnGPU()
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

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Bind()
	{
		Bind(gfx.GetContextInterface());
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		Bind_(static_cast<const DX11Context*>(&context)->GetContext());
	}

	template <BufferType Type, CachingPolicy Policy>
	ID3D11Buffer* DX11Buffer<Type, Policy>::Data() const
	{
		return pBuffer.Get();
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Update()
	{
		Update(gfx.GetContext());
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Update(ID3D11DeviceContext& context)
	{
		D3D11_MAPPED_SUBRESOURCE msr;

		GPUSizeChanges();

		context.Map(
			Data(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		) >> chk;

		memcpy(msr.pData, this->pCPUBuffer->Data(), this->pCPUBuffer->ByteSize());

		stride = (UINT)this->pCPUBuffer->Stride();

		context.Unmap(Data(), 0u);
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::GPUSizeChanges() requires (Type == BufferType::Instance)
	{
		if (this->gpuSize < this->pCPUBuffer->ByteSize() ||
			this->gpuSize * 0.6 > this->pCPUBuffer->ByteSize())
		{
			InitDynamicCBufferOnGPU();
		}
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::GPUSizeChanges() requires (Type != BufferType::Instance)
	{
		return;
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Bind_(ID3D11DeviceContext& context) requires (Type == BufferType::Vertex)
	{
		context.IASetVertexBuffers((UINT)0, (UINT)1, pBuffer.GetAddressOf(), &stride, &offset);
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Bind_(ID3D11DeviceContext& context) requires (Type == BufferType::Index)
	{
		context.IASetIndexBuffer(pBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	template <BufferType Type, CachingPolicy Policy>
	void DX11Buffer<Type, Policy>::Bind_(ID3D11DeviceContext& context) requires (Type == BufferType::PxConstant || Type
		== BufferType::VtxConstant || Type == BufferType::Instance)
	{
		if (this->pCPUBuffer->Dirty())
		{
			Update(context);
			this->pCPUBuffer->SetClean();
		}
		if constexpr (Type == BufferType::PxConstant)
		{
			context.PSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
		}
		else
		{
			context.VSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
		}
	}

	template <BufferType Type, CachingPolicy Policy>
	std::vector<std::any> DX11Buffer<Type, Policy>::GetLayoutFromVB() const
	{
		trynass_msg(Type == BufferType::Vertex, L"Can only get the layout from a Vertex Buffer Type!");
		return GetSlottedLayoutFromVB(0);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::vector<std::any> DX11Buffer<Type, Policy>::GetSlottedLayoutFromVB(int slot) const
	{
		trynass_msg(Type == BufferType::Vertex,L"Can only get the layout from a Vertex Buffer Type!");
		return GetSlottedLayoutFromVB_(slot);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::string_view DX11Buffer<Type, Policy>::Test() const
	{
		return "hello from dx11Buffer";
	}

	template <BufferType Type, CachingPolicy Policy>
	std::vector<std::any> DX11Buffer<Type, Policy>::GetSlottedLayoutFromVB_(int slot) const requires (Type == BufferType
		::Vertex)
	{
		trynass_msg(Type == BufferType::Vertex, L"Can only get layouts from Vertex Buffer Types!");
		const auto& vLayout = IBufferBase<Type, Policy>::GetLayout();
		const auto descSize = vLayout.GetElementCount();

		std::vector<std::any> layout;
		for (int i = 0; i < descSize; i++)
		{
			D3D11_INPUT_ELEMENT_DESC descriptor = {};
			descriptor.SemanticName = vLayout.Elements[i].GetName().data();
			descriptor.SemanticIndex = vLayout.Elements[i].Index();
			descriptor.Format = Graphics::MapDXGIFormat(vLayout.Elements[i].GetFormat());
			descriptor.InputSlot = (UINT)slot;
			descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descriptor.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			descriptor.InstanceDataStepRate = 0u;
			layout.push_back(descriptor);
		}

		return layout;
	}

	template <BufferType Type, CachingPolicy Policy>
	std::vector<std::any> DX11Buffer<Type, Policy>::GetSlottedLayoutFromVB_(int slot)
	{
		throw BufferMissmatchException(L"Incompatible buffer type call!");
	}

	template class DX11Buffer<BufferType::Index,		CachingPolicy::Caching>;
	template class DX11Buffer<BufferType::VtxConstant,	CachingPolicy::Caching>;
	template class DX11Buffer<BufferType::Vertex,		CachingPolicy::Caching>;
	template class DX11Buffer<BufferType::PxConstant,	CachingPolicy::Caching>;
	template class DX11Buffer<BufferType::Instance,		CachingPolicy::Caching>;
	template class DX11Buffer<BufferType::Index,		CachingPolicy::NonCaching>;
	template class DX11Buffer<BufferType::VtxConstant,	CachingPolicy::NonCaching>;
	template class DX11Buffer<BufferType::Vertex,		CachingPolicy::NonCaching>;
	template class DX11Buffer<BufferType::PxConstant,	CachingPolicy::NonCaching>;
	template class DX11Buffer<BufferType::Instance,		CachingPolicy::NonCaching>;
}
