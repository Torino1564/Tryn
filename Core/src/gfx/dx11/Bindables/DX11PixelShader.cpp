#include "DX11PixelShader.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/src/utl/String.h>

#include <d3dcompiler.h>
#pragma comment (lib,"D3DCompiler.lib")

namespace tryn::gfx::dx11
{
	DX11PixelShader::DX11PixelShader(Graphics& gfx, std::string path)
		:
		gfx(gfx)
	{
		type = GraphicAPI::DX11;

		D3DReadFileToBlob(utl::ToWide(path).c_str(), &pBlob) >> chk;

		gfx.GetDevice()->CreatePixelShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pPS
		) >> chk;
	}
	void DX11PixelShader::Bind()
	{
		gfx.GetContext()->PSSetShader(pPS.Get(), nullptr, 0u);
	}
}