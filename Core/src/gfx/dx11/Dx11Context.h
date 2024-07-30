#pragma once
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/IContext.h>
#include <d3d11_1.h>
#include <Core/src/utl/Assert.h>

namespace tryn::gfx::dx11
{
	class Graphics;
	class DX11Context : public IContext
	{
	friend class Graphics;
	public:
		DX11Context() = default;
		DX11Context(const Graphics& gfx);
		ID3D11DeviceContext& GetContext() const;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetCOMPtr();
		constexpr GraphicAPI GetApi() const override;
		void Submit(const IGraphics& gfx) override;
		void DrawIndexed(int count) const override;
		void DrawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) const override;
	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pView;
		Microsoft::WRL::ComPtr<ID3D11CommandList> pCommandList;
	};
}