#include "Dx11Context.h"
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/Bindables/DX11Buffer.h>

namespace tryn::gfx::dx11
{
	DX11Context::DX11Context(Graphics& gfx)
	{
		// Create context
		gfx.GetDevice().CreateDeferredContext(0u, pContext.GetAddressOf());
		deferred = true;
		// Set Render Target
		pContext->OMSetRenderTargets(1u, gfx.pTarget.GetAddressOf(), gfx.pDSV.Get());
		// Set Viewport
		pContext->RSSetViewports(1u, &gfx.viewport);

		// Init Constant Buffer
		{
			ConstantBufferLayout cblayout;
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "model"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "view"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "viewProjection"));
			cblayout.Solidify();
			pTCB = std::make_unique<DX11VtxConstantBuffer>(gfx, std::move(cblayout), 0, "transformCbuf");
		}
	}
	ID3D11DeviceContext& DX11Context::GetContext()
	{
		return *pContext.Get();
	}
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& DX11Context::GetCOMPtr()
	{
		return pContext;
	}
	constexpr GraphicAPI DX11Context::GetApi() const
	{
		return GraphicAPI::DX11;
	}
	void DX11Context::Submit(IGraphics& gfx)
	{
		trynass_msg(deferred, L"Called the submit member on a non deferred context");
		gfx.AssertContextCoherence(*this);
		auto& immediateContext = static_cast<DX11Context&>(gfx.GetContextInterface()).GetContext();
		pContext->FinishCommandList(TRUE, &pCommandList) >> chk;
		immediateContext.ExecuteCommandList(pCommandList.Get(), TRUE);
	}
	void DX11Context::DrawIndexed(int count)
	{
		pContext->DrawIndexed(count, 0u, 0u);
	}
}

