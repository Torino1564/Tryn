#pragma once
#include <Core/src/gfx/Bindables/PolyInputLayout.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include "DX11PolyVBuffer.h"
#include "DX11VertexShader.h"

namespace tryn::gfx::dx11
{
	class DX11PolyInputLayout : public PolyInputLayout
	{
	public:
		DX11PolyInputLayout(Graphics&, DX11PolyVBuffer&, DX11VertexShader& );
	private:
		Graphics& gfx;
		DX11VertexShader& vs;
	};
}