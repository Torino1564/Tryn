#pragma once
#include <Core/src/gfx/Bindables/Vertex.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <memory>

namespace tryn::gfx::dx11
{
	class DX11VertexBuffer : public VertexBuffer
	{
	public:

		DX11VertexBuffer(Graphics& gfx, VertexLayout layout_, size_t size);
		void Bind() override;
		std::vector<char> GetLayoutFromVB() const override;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		Graphics& gfx;
	};
}