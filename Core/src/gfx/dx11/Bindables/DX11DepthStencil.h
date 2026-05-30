#pragma once
#include <Core/src/gfx/bindables/DepthStencil.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11DepthStencil : public IDepthStencil
	{
	public:
		ID3D11DepthStencilView* Get() const;
		ID3D11DepthStencilView** GetAddressOf();
		DX11DepthStencil(const Graphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, ComparissonMode mode);
		void Bind() override;
		void Bind(const class IContext& ctx) override;
		void Clear() const override;
		void Release() const;

		void RegenerateResources(const spa::DimensionsI dimensions) override;
	private:
		void DSVCreation(const Graphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode, bool isShaderResource);
		void SRVCreation(const Graphics& gfx, const uint16_t slot);

		ComparissonMode mode = ComparissonMode::Less;
		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	};
}
