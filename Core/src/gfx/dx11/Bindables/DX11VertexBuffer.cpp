#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>

namespace tryn::gfx::dx11
{
	DX11VertexBuffer::DX11VertexBuffer(const Graphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag)
		: gfx(gfx)
	{
		this->type = GraphicAPI::DX11;
		this->tag = tag;
		trynass_msg(!cpuBuffer->Dirty(), L"Cant initialize a dirty Vertex Buffer!");
		this->pCPUBuffer = cpuBuffer;
		stride = (UINT)IBufferBase<BufferType::Vertex, CachingPolicy::Caching>::pCPUBuffer->Stride();
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
		gfx.GetContext().Bind(*this);
	}
	void DX11VertexBuffer::Bind(const IContext& context)
	{
		context.Bind(*this);
	}
	std::vector<std::any> DX11VertexBuffer::GetLayoutFromVB() const
	{
		return GetLayout().Elements;
	}
	std::vector<std::any> DX11VertexBuffer::GetSlottedLayoutFromVB(int slot) const
	{
		return GetLayout().Elements;
	}
	void DX11VertexBuffer::Resize(const std::size_t newSize)
	{
		trynass_fail(L"Resizing a vertex buffer is not supported currently");
	}
	ID3D11Buffer* DX11VertexBuffer::Data() const
	{
		return pBuffer
	}
}