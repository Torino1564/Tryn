#pragma once
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <memory>
#include <vector>

namespace tryn::gfx::dx11
{

	class DX11IndexBuffer : public IIndexBuffer
	{
	public:
		DX11IndexBuffer(Graphics& gfx, std::shared_ptr<const std::vector<int>> indices, std::string tag);
		void Bind( ) override;
		const size_t Size() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		std::shared_ptr<const std::vector<int>> indices;
		int count;
		Graphics& gfx;
	};
}
