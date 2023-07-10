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
		for (auto& vertexBuffer : polyVB.slots)
		{
			auto dx11VertexBuffer = static_cast<DX11VertexBuffer*>(vertexBuffer.get());
			slots.push_back(std::make_shared<DX11InputLayout>(gfx, *dx11VertexBuffer, vs));
		}
	}
}

