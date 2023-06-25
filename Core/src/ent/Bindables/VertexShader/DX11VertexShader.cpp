#include "DX11VertexShader.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <d3dcompiler.h>
#pragma comment (lib,"D3DCompiler.lib")


namespace tryn::ent::bnd
{
	DX11VertexShader::DX11VertexShader(gfx::IGraphics gfx , std::wstring path)
	{
		D3DReadFileToBlob(path.c_str(), &pBlob) >> gfx::dx11::chk;
		gfx.GetDevice()->CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVS) >> gfx::dx11::chk;
	}
	DX11VertexShader::~DX11VertexShader() {}

	void DX11VertexShader::Bind(gfx::IGraphics& gfx)
	{

	}
}


