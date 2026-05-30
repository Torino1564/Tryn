#pragma once
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11VertexShader : public IVertexShader
	{
	public:
		DX11VertexShader(const Graphics& gfx, const std::string& path);
		static DX11VertexShader Constructor(auto&&... args)
		{
			return DX11VertexShader(args...);
		}
		void Bind() override;
		void Bind(const IContext& context) override;
		const Microsoft::WRL::ComPtr<ID3DBlob>& GetBlob() const;
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVS;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		const Graphics& gfx;
	};
}