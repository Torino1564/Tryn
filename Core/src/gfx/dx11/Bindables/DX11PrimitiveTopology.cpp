#include "DX11PrimitiveTopology.h"

namespace tryn::gfx::dx11
{
	DX11PrimitiveTopology::DX11PrimitiveTopology(Graphics& gfx , D3D_PRIMITIVE_TOPOLOGY type)
		:
		gfx(gfx),
		type(type)
	{}
	void DX11PrimitiveTopology::Bind()
	{
		gfx.GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

