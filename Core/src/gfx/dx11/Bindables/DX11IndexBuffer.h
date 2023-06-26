#pragma once
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <vector>
#include <memory>

namespace tryn::ent
{
	class Model;
}

namespace tryn::gfx::dx11
{

	class DX11IndexBuffer : public BaseIndexBuffer
	{
	public:
		DX11IndexBuffer( gfx::IGraphics& gfx, tryn::ent::Model& model);
		void Bind( gfx::IGraphics& gfx ) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		std::shared_ptr<std::vector<int>> pIndeces;
		int count;
	};
}
