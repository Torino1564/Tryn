#pragma once
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11PolyVBuffer : public IPolyVBuffer
	{
	public:
		DX11PolyVBuffer(Graphics& gfx);
		void Append(std::shared_ptr<VertexBuffer>) override;
		void Bind() override;
	private:
		Graphics& gfx;
		bool initialized = false;
		std::vector<UINT> strides;
		std::vector<UINT> offsets;
		std::vector<ID3D11Buffer*> buffArray;
	};
}