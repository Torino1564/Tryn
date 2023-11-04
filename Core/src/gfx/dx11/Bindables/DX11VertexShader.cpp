#include "DX11VertexShader.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <d3dcompiler.h>
#include <core/src/utl/String.h>
#pragma comment (lib,"D3DCompiler.lib")
#include <Core/src/utl/String.h>

namespace tryn::gfx::dx11
{
	DX11VertexShader::DX11VertexShader(Graphics& gfx, std::string& path)
		:
		gfx(gfx)
	{
		type = GraphicAPI::DX11;
		this->path = path;

		D3DReadFileToBlob(utl::ToWide(path).c_str(), &pBlob) >> chk;
		gfx.GetDevice().CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVS) >> chk;
	}

	void DX11VertexShader::Bind()
	{
		gfx.GetContext().VSSetShader(pVS.Get(), nullptr, 0u);
	}
	const Microsoft::WRL::ComPtr<ID3DBlob>& DX11VertexShader::GetBlob() const
	{
		return pBlob;
	}
}