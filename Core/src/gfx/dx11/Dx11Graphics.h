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
		Graphics(HWND hWnd, int width, int height);
		~Graphics() override;
		void BeginFrame() override;
		void EndFrame() override;
		void ClearBuffer(float r = 0, float g = 0, float b = 0) override;
		void DrawTriangle() override;
		void DrawIndexed(int count) override;
		GraphicAPI GetType() override;
		void MakeBindablesForModel(ent::Model&) override;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();
		Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice();

		// Resurce Creation
		std::shared_ptr<IVertexBuffer>			CreateVertexBuffer(std::shared_ptr<VertexBuffer> cpuBuffer) override;
		std::shared_ptr<IPolyVBuffer>			CreatePolyVertexBuffer(std::vector<std::shared_ptr<VertexBuffer>>& buffers) override;
		std::shared_ptr<IIndexBuffer>			CreateIndexBuffer(std::shared_ptr<std::vector<int>> indices) override;
		std::shared_ptr<IVertexShader>			CreateVertexShader(std::string path) override;
		std::shared_ptr<IPixelShader>			CreatePixelShader(std::string path) override;
		std::shared_ptr<IInputLayout>			CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) override;
		std::shared_ptr<IInputLayout>			CreateInputLayout(IPolyVBuffer& vb, IVertexShader& vs) override;
		std::shared_ptr<IPrimitiveTopology>		CreatePrimitiveTopology() override;
		std::shared_ptr<IConstantBuffer>		CreateConstantBuffer(ConstantBufferLayout&&) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	};

}