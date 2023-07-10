#pragma once
#include "TrynWLR.h"
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/spa/Dimensions.h>
#include <d3d11.h>

namespace tryn::gfx
{
	class VertexBuffer;
}

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
		void DrawTriangle() override;
		void DrawIndexed(int count) override;
		GraphicAPI GetType() override;
		void MakeBindablesForModel(ent::Model&) override;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();
		Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice();

		// Resurce Creation
		void CreateVertexBuffer(std::shared_ptr<VertexBuffer> cpuBuffer, IBindable** pBindable) override;
		/*void CreateVertexShader() override;
		void CreateIndexBuffer() override;
		void CreatePixelShader() override;
		void CreateConstantBuffer() override;
		void CreatePrimitiveTopology() override;*/

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	};

}