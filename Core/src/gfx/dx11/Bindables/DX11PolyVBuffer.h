#pragma once
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11PolyVBuffer : public IPolyVBuffer
	{
	public:
		DX11PolyVBuffer(Graphics& gfx);
		void Append(std::shared_ptr<IVertexBuffer>) override;
		void Bind() override;
	private:
		Graphics& gfx;
	};
}