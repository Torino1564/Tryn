#pragma once
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/IContext.h>
#include <d3d11_1.h>

namespace tryn::gfx::dx11
{
	class DX11Context : public IContext
	{
	friend class Graphics;
	public:
		DX11Context()
		{

		}
		ID3D11DeviceContext& GetContext()
		{
			return *pContext.Get();
		}
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetCOMPtr()
		{
			return pContext;
		}
		constexpr GraphicAPI GetApi() const override
		{
			return GraphicAPI::DX11;
		}
		ID3D11Asynchronous& GetAsync()
		{
			return *pAsync.Get();
		}
		void Submit() override
		{
			trynass_msg(deferred, L"Called the submit member on a non deferred context");

			//pContext->End(pAsync.Get());
			ID3D11CommandList* pCommandList = nullptr;
			pContext->FinishCommandList(FALSE, &pCommandList);
			pContext->ExecuteCommandList(pCommandList, FALSE);
		}
		void DrawIndexed(int count) override
		{
			pContext->DrawIndexed(count, 0u, 0u);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pView;
		Microsoft::WRL::ComPtr<ID3D11Asynchronous> pAsync;
	};
}