#include "DX11VertexShader.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <d3dcompiler.h>
#pragma comment (lib,"D3DCompiler.lib")


namespace tryn::gfx::dx11
{
	DX11VertexShader::DX11VertexShader(gfx::IGraphics& gfx , std::wstring path)
	{
		trynass_msg(gfx.GetType() == gfx::Type::DX11, L"DX11 Vertex Shader called with a reference to a different graphics api");

		gfx::dx11::Graphics& rGfx = gfx.QueryInterface<gfx::dx11::Graphics>();

		D3DReadFileToBlob(path.c_str(), &pBlob) >> gfx::dx11::chk;
		rGfx.GetDevice()->CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVS) >> gfx::dx11::chk;
	}
	DX11VertexShader::~DX11VertexShader() {}

	void DX11VertexShader::Bind( gfx::IGraphics& gfx )
	{
		trynass_msg(gfx.GetType() == gfx::Type::DX11, L"DX11 Vertex Shader called with a reference to a different graphics api");

		gfx::dx11::Graphics& rGfx = dynamic_cast<gfx::dx11::Graphics&>(gfx);

		rGfx.GetContext()->VSSetShader(pVS.Get(), nullptr, 0u);
	}
}


