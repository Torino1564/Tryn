#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/Dx11Context.h>

namespace tryn::gfx::dx11
{
	DX11VertexBuffer::DX11VertexBuffer(const Graphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag)
		: gfx(gfx)
	{
		this->type = GraphicAPI::DX11;
		this->tag = tag;
		trynass_msg(!cpuBuffer->Dirty(), L"Cant initialize a dirty Vertex Buffer!");
		this->pCPUBuffer = cpuBuffer;
		stride = (UINT)this->pCPUBuffer->Stride();
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = stride;
		bd.ByteWidth = (UINT)this->pCPUBuffer->ByteSize();
		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = this->pCPUBuffer->Data();
		gfx.GetDevice().CreateBuffer(&bd, &srd, &pBuffer) >> chk;
	}
	void DX11VertexBuffer::Bind()
	{
		Bind(gfx.GetContextInterface());
	}
	void DX11VertexBuffer::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		Bind_(static_cast<const DX11Context*>(&context)->GetContext());
	}
	std::vector<std::any> DX11VertexBuffer::GetLayoutFromVB() const
	{
		return GetSlottedLayoutFromVB(0);
	}
	std::vector<std::any> DX11VertexBuffer::GetSlottedLayoutFromVB(int slot) const
	{
		const auto& vLayout = GetLayout();
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
	ID3D11Buffer* DX11VertexBuffer::Data() const
	{
		return pBuffer.Get();
	}
	void DX11VertexBuffer::Bind_(ID3D11DeviceContext& context)
	{
		context.IASetVertexBuffers((UINT)0, (UINT)1, pBuffer.GetAddressOf(), &stride, &offset);
	}
}