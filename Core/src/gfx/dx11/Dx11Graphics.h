#pragma once
#include "TrynWLR.h"
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/spa/Dimensions.h>
#include <d3d11_1.h>

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
		void DrawIndexed(int count) override;
		GraphicAPI GetType() override;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>& GetContext();
		Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice();

		// Resurce Creation
		std::shared_ptr<IVertexBuffer>			CreateVertexBuffer(std::shared_ptr<VertexBuffer>, std::string) override;
		std::shared_ptr<IPolyVBuffer>			CreatePolyVertexBuffer(std::vector<std::variant<std::pair<std::string, std::shared_ptr<tryn::gfx::VertexBuffer>>, std::shared_ptr<tryn::gfx::IVertexBuffer>>>& CpuVBs, std::string) override;
		std::shared_ptr<IIndexBuffer>			CreateIndexBuffer(std::shared_ptr<const std::vector<int>> indices, std::string) override;
		std::shared_ptr<IVertexShader>			CreateVertexShader(std::string path) override;
		std::shared_ptr<IPixelShader>			CreatePixelShader(std::string path) override;
		std::shared_ptr<IInputLayout>			CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) override;
		std::shared_ptr<IInputLayout>			CreateInputLayout(IPolyVBuffer& vb, IVertexShader& vs) override;
		std::shared_ptr<IInputLayout>			CreateInputLayout(StaticMesh& mesh, IVertexShader& vs) override;
		std::shared_ptr<IPrimitiveTopology>		CreatePrimitiveTopology() override;
		std::shared_ptr<IConstantBuffer>		CreateConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	};

}