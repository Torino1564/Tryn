#include "DX11IndexBuffer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/src/ent/Model.h>

namespace tryn::gfx::dx11
{
	DX11IndexBuffer::DX11IndexBuffer(gfx::IGraphics& gfx, tryn::ent::Model& model)
	{
		auto rGfx = gfx.QueryInterface<dx11::Graphics>();

		pIndeces = std::make_shared<std::vector<int>>(model.indices);
		count = (int)model.indices.size();

		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.StructureByteStride = sizeof(int);
		ibd.ByteWidth = count;
		D3D11_SUBRESOURCE_DATA isrd = {};
		isrd.pSysMem = pIndeces.get();

		rGfx->GetDevice()->CreateBuffer(&ibd, &isrd, &pBuffer) >> gfx::dx11::chk;
	}
	void DX11IndexBuffer::Bind(gfx::IGraphics& gfx)
	{
		auto rGfx = gfx.QueryInterface<dx11::Graphics>();

		rGfx->GetContext()->IASetIndexBuffer(pBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}
}