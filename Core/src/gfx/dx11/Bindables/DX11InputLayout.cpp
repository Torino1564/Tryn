#include "DX11InputLayout.h"
#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11InputLayout::DX11InputLayout(Graphics& gfx, DX11VertexBuffer& vb, DX11VertexShader& vs)
		:
		gfx(gfx)
	{
		auto layoutBuf = vb.GetLayoutFromVB();
		auto layout = reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(layoutBuf.data());

		gfx.GetDevice()->CreateInputLayout(layout, (UINT)vb.ConstGet().GetLayout().GetElementCount(), vs.GetBlob()->GetBufferPointer(), vs.GetBlob()->GetBufferSize(), &pLayout) >> chk;
	}
	void DX11InputLayout::Bind()
	{
		gfx.GetContext()->IASetInputLayout(pLayout.Get());
	}
}
