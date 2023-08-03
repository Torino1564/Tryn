#pragma once
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <vector>

struct D3D11_INPUT_ELEMENT_DESC;

namespace tryn::gfx::dx11
{
	class DX11VertexBuffer;
	class DX11VertexShader;
	class DX11PolyVBuffer;
	class DX11InputLayout : public IInputLayout
	{
	public:
		DX11InputLayout(Graphics& gfx, IVertexBuffer& vb, IVertexShader& vs);
		DX11InputLayout(Graphics& gfx, IPolyVBuffer& pvb, IVertexShader& vs);
		DX11InputLayout(Graphics& gfx, VertexLayout& layout, IVertexShader& vs);
		void Bind() override;

	private:
		Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> buffer;
	};
}