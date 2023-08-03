#pragma once
#include <Core/src/gfx/Bindables/ConstantBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>

namespace tryn::gfx::dx11
{
	class DX11VtxConstantBuffer : public IVtxConstantBuffer
	{
	public:
		DX11VtxConstantBuffer(Graphics& gfx, ConstantBufferLayout&& cbl, int slot, std::string tag = "?");
		void Bind() override;
		char* Data();
		void Update();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuff;
		Graphics& gfx;
	};

	class DX11PxConstantBuffer : public IPxConstantBuffer
	{
	public:
		DX11PxConstantBuffer(Graphics& gfx, ConstantBufferLayout&& cbl, int slot, std::string tag = "?");
		void Bind() override;
		char* Data();
		void Update();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuff;
		Graphics& gfx;
	};
}