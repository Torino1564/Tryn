#include "DX11IndexBuffer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/src/utl/Assert.h>
#include <Core/src/ent/Model/Model.h>

namespace tryn::gfx::dx11
{
	DX11IndexBuffer::DX11IndexBuffer(Graphics& gfx, std::string tag, std::shared_ptr<const std::vector<int>> indices_)
		:
		gfx(gfx)
	{
		this->tag = tag;

		indices = indices_;
		count = (int)indices->size();
		type = GraphicAPI::DX11;
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.StructureByteStride = sizeof(int);
		ibd.ByteWidth = count * sizeof((*indices)[0]);
		D3D11_SUBRESOURCE_DATA isrd = {};
		isrd.pSysMem = indices->data();

		gfx.GetDevice()->CreateBuffer(&ibd, &isrd, &pBuffer) >> chk;
	}
	void DX11IndexBuffer::Bind()
	{
		gfx.GetContext()->IASetIndexBuffer(pBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}
	const size_t DX11IndexBuffer::Size() const
	{
		return indices->size();
	}
}