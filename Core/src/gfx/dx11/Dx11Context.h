#pragma once
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/IContext.h>
#include <d3d11_1.h>

namespace tryn::gfx::dx11
{
	class DX11Context : public IContext
	{
	friend class Graphics;
	public:
		ID3D11DeviceContext& GetContext()
		{
			return *pContext.Get();
		}
		constexpr GraphicAPI GetApi() const override
		{
			return GraphicAPI::DX11;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	};
}