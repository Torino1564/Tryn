#pragma once
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <d3d11.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <core/src/gfx/dx11/Dx11Graphics.h>
#include <string>

namespace tryn::gfx::dx11
{
	class DX11VertexShader : public BaseVertexShader
	{
	public:
		DX11VertexShader(gfx::IGraphics& gfx , std::wstring path);
		~DX11VertexShader() override;
		void Bind(gfx::IGraphics& gfx) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVS;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
};
}