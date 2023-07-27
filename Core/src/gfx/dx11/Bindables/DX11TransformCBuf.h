#pragma once
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Bindables/TransformCBuf.h>

namespace tryn::gfx::dx11
{
	class DX11TransformCBuf : public ITransformCBuf
	{
	public:
		DX11TransformCBuf(Graphics& gfx);
	};
}
