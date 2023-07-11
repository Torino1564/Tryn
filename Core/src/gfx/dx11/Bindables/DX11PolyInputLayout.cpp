#include "DX11PolyInputLayout.h"
#include "DX11InputLayout.h"
#include "DX11VertexBuffer.h"

namespace tryn::gfx::dx11
{
	DX11PolyInputLayout::DX11PolyInputLayout(Graphics& gfx, DX11PolyVBuffer& polyVB, DX11VertexShader& vs)
		:
		gfx(gfx),
		vs(vs)
	{
		
	}
	void DX11PolyInputLayout::Bind()
	{
	}
}

