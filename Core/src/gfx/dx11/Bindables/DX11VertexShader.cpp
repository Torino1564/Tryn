#include "DX11VertexShader.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <d3dcompiler.h>
#pragma comment (lib,"D3DCompiler.lib")


namespace tryn::gfx::dx11
{
	DX11VertexShader::DX11VertexShader(Graphics& gfx , std::wstring path)
		:
		gfx(gfx)
	{
		D3DReadFileToBlob(path.c_str(), &pBlob) >> chk;
		gfx.GetDevice()->CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVS) >> chk;
	}

	void DX11VertexShader::Bind( )
	{
		gfx.GetContext()->VSSetShader(pVS.Get(), nullptr, 0u);
	}
	const Microsoft::WRL::ComPtr<ID3DBlob>& DX11VertexShader::GetBlob() const
	{
		return pBlob;
	}
}


