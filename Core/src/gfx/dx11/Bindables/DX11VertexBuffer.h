#pragma once
#include <Core/src/gfx/Bindables/VertexBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <memory>

namespace tryn::gfx::dx11
{
	class DX11VertexBuffer : public IVertexBuffer
	{
	public:

		DX11VertexBuffer(Graphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer);
		void Bind() override;
		std::vector<char> GetLayoutFromVB() const override;
		std::vector<char> GetSlottedLayoutFromVB( int slot = 0 ) const override;
		void BindSlotted(int slot, int buffCount) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		Graphics& gfx;
	};
}