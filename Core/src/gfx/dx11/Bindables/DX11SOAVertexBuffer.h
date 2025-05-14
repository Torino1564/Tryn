#pragma once
#include <Core/src/gfx/Bindables/SOAVertexBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11VertexShader;
	class DX11InputLayout;

	class DX11SOAVertexBuffer : public ISOAVertexBuffer
	{
	public:
		DX11SOAVertexBuffer(const Graphics& gfx, const std::shared_ptr<IVertexShader>& pVS);
		void Bind() override;
		void Bind(const IContext& context) override;

	private:
		void AssertApiMatch(const std::shared_ptr<IVertexBuffer>& pVB) override;
		void Bind_Impl(const IContext& context);
		void Update();

		const Graphics& gfx;
		bool initialized = false;
		std::vector<UINT> strides;
		std::vector<UINT> offsets;
		std::vector<ID3D11Buffer*> buffArray;
		std::shared_ptr<DX11InputLayout> pLayout;
		std::shared_ptr<DX11VertexShader> pVS;
	};
}