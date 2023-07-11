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
		void CreateVertexBuffer(std::shared_ptr<VertexBuffer> cpuBuffer, IVertexBuffer** ppBindable) override;
		void CreatePolyVertexBuffer(std::vector<std::shared_ptr<VertexBuffer>>& buffers, IPolyVBuffer** ppBindable) override;
		void CreateIndexBuffer(std::shared_ptr<std::vector<int>> indices, IIndexBuffer** ppBindable) override;
		void CreateVertexShader(std::wstring path, IVertexShader** ppBindable) override;
		void CreatePolyInputLayout(IPolyVBuffer& pVBuf, IVertexShader& vs, IPolyInputLayout** ppBindable) override;
		void CreatePixelShader(std::wstring path, IPixelShader** ppBindable) override;
		void CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs, IInputLayout** ppBindable) override;
		void CreateInputLayout(IPolyVBuffer& vb, IVertexShader& vs, IInputLayout** ppBindable) override;
		void CreatePrimitiveTopology(IPrimitiveTopology** ppBindable) override;
		void CreateConstantBuffer(ConstantBufferLayout&&, IConstantBuffer** ppBindable) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	};

}