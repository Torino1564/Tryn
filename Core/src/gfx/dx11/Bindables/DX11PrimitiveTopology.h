#pragma once
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11PrimitiveTopology : public IPrimitiveTopology
	{
	public:
		DX11PrimitiveTopology(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY type = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Bind() override;
	private:
		D3D_PRIMITIVE_TOPOLOGY topologyType;
		Graphics& gfx;
	};
}