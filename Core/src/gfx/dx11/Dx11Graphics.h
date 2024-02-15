#pragma once
#include "TrynWLR.h"
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/IGraphics.h>
#include <d3d11_1.h>
#include <Core/src/gfx/dx11/Dx11Context.h>
#include <Core/third/stb_image/stb_image.h>

namespace tryn::gfx
{
	class VertexBuffer;
}

namespace tryn::win
{
	class Window;
}

namespace tryn::gfx::dx11
{
	class Graphics : public IGraphics
	{
		friend class DX11Context;
	public:
		Graphics(HWND hWnd, int width, int height);
		~Graphics() override;
		void BeginFrame() override;
		void EndFrame() override;
		void ClearBuffer(float r = 0, float g = 0, float b = 0) override;
		void DrawIndexed(int count) override;
		void DrawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) override;
		constexpr GraphicAPI GetType() const override;
		ID3D11DeviceContext& GetContext();
		ID3D11Device& GetDevice();
		IDXGISwapChain& GetSwapChain();
		constexpr const char* GetAPIString() const override;

		static constexpr DXGI_FORMAT MapDXGIFormat(VertexLayout::Format format)
		{
			switch (format)
			{
			case VertexLayout::Format::Vec2F:
				return DXGI_FORMAT_R32G32_FLOAT;
				break;
			case VertexLayout::Format::Vec3F:
				return DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VertexLayout::Format::Vec4F:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case VertexLayout::Format::Vec4C_UNorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			case VertexLayout::Format::Float_Uint:
				return DXGI_FORMAT_R32_UINT;
				break;
			case VertexLayout::Format::Vec4UI16:
				return DXGI_FORMAT_R16G16B16A16_UINT;
				break;
			}
			return DXGI_FORMAT_UNKNOWN;
		}
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetSlottedLayout(const VertexLayout& vLayout, int slot);

		// Resource Creation
		std::shared_ptr<IVertexBuffer>						CreateVertexBuffer(std::shared_ptr<VertexBuffer>, std::string) override;
		std::shared_ptr<IIndexBuffer>						CreateIndexBuffer(std::shared_ptr<IndexBuffer> indices, std::string) override;
		std::shared_ptr<IVertexShader>						CreateVertexShader(std::string path) override;
		std::shared_ptr<IPixelShader>						CreatePixelShader(std::string path) override;
		std::shared_ptr<IInputLayout>						CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) override;
		std::shared_ptr<IInputLayout>						CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs) override;
		std::shared_ptr<IPrimitiveTopology>					CreatePrimitiveTopology() override;
		std::shared_ptr<IVtxConstantBuffer>					CreateVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") override;
		std::shared_ptr<IVtxConstantBufferNCach>			CreateNonCachVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") override;
		std::shared_ptr<IPxConstantBuffer>					CreatePxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") override;
		std::shared_ptr<IPxConstantBufferNCach>				CreateNonCachPxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") override;
		std::unique_ptr<IInstanceBuffer>					CreateInstanceBuffer(ConstantBufferLayout::Node node, std::size_t size, int slot = 2) override;
		std::shared_ptr<ITexture>							CreateTexture(std::filesystem::path path, int slot = 0) override;
		std::shared_ptr<IRasterizer>						CreateRasterizer(const bool twoSided = true) override;
		std::shared_ptr<ISampler>							CreateSampler(SamplerType type, bool reflect, int slot) override;
		std::shared_ptr<IOutputOnlyRenderTargetView>		CreateOutputOnlyRenderTargetView(const spa::DimensionsI dimensions) override;
		std::shared_ptr<IShaderResourceRenderTargetView>	CreateShaderResourceRenderTargetView(const spa::DimensionsI, const uint16_t slot) override;
		std::shared_ptr<IOutputOnlyDepthStencil>			CreateOutputOnlyDepthStencil(const spa::DimensionsI, ComparissonMode mode = ComparissonMode::Less) override;
		std::shared_ptr<IShaderResourceDepthStencil>		CreateShaderResourceDepthStencil(const spa::DimensionsI, const uint16_t slot, ComparissonMode mode) override;
		std::unique_ptr<ITransformCBuf>						CreateTransformCBuf() override;
		std::unique_ptr<RenderWorker>						CreateRenderWorker(ccr::Master*) override;

	private:

		static constexpr const char* APIString = "DX11";

		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
		D3D11_VIEWPORT viewport;
	};
}