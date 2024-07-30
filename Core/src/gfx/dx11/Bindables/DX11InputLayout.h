#pragma once
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <vector>
#include <Core/src/gfx/dx11/DX11BufferFwd.h>

struct D3D11_INPUT_ELEMENT_DESC;

namespace tryn::gfx::dx11
{
	class DX11VertexShader;
	class DX11InputLayout : public IInputLayout
	{
	public:
		DX11InputLayout(const Graphics& gfx, IVertexBuffer& vb, IVertexShader& vs);
		DX11InputLayout(const Graphics& gfx, VertexLayout& layout, IVertexShader& vs);
		void Bind() override;
		void Bind(const IContext& context) override;

	private:
		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> buffer;
	};
}