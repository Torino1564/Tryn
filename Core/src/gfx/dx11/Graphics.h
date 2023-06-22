#pragma once
#include <Core/src/win/TrynWin.h>
#include <Core/src/gfx/IGraphics.h>
#include <d3d11.h>
#include "TrynWLR.h"

namespace tryn::gfx::dx11
{

	class Graphics : public IGraphics
	{
	public:
		Graphics( HWND hWnd, int width, int height );
		~Graphics() override;
		void BeginFrame() override;
		void EndFrame() override;
		void ClearBuffer(float r, float g, float b) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;

	};

}